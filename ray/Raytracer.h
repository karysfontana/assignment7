#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "HitRecord.h"
#include "Ray.h"
#include "../sgraph/RaycastRenderer.h"
#include "../sgraph/IScenegraph.h"
#include <stack>
#include <iostream>
using namespace std;

namespace ray {
class Raytracer {
    public:
    Raytracer(int w, int h, 
              stack<glm::mat4>& mv,
              map<string, util::PolygonMesh<VertexAttrib>>& meshes,
              sgraph::IScenegraph* sg)
        : width(w), height(h), modelview(mv), scenegraph(sg), fov(60.0f) {
        image.resize(width * height * 3);
        renderer = new sgraph::RaycastRenderer(mv, meshes);
    }
    
    ~Raytracer() {
        delete renderer;
    }
    
    void setTextureImages(map<string, unsigned int> texImages) {
        renderer->setTextureMap(texImages);
    }
    
    // generate a ray for current pixel
    Ray generateRay(int i, int j) {
        float aspect = (float)width / height;
        float tanFov = tan(glm::radians(fov / 2.0f));        
        float x = (2.0f * (i + 0.5f) / width - 1.0f) * aspect * tanFov;
        float y = (1.0f - 2.0f * (j + 0.5f) / height) * tanFov;
        glm::vec3 origin(0.0f, 0.0f, 0.0f);
        glm::vec3 direction = glm::normalize(glm::vec3(x, y, -1.0f));
        return Ray(origin, direction);
    }

    // Determines if a given hit point is in shadow.
    bool inShadow(const glm::vec3& point, const glm::vec3& normal, const util::Light* light) {
        glm::vec3 direction;
        float max;
        if (light->getSpotCutoff() == 0.0f) {
            glm::vec3 position = glm::vec3(light->getPosition()) - point;
            max = glm::length(position);    
            direction = position / max;
        } else {
            direction = glm::normalize(-glm::vec3(light->getSpotDirection()));
            max = std::numeric_limits<float>::max();
        }        
        glm::vec3 shadow = point + normal * 0.001f;
        Ray shadowRay(shadow, direction);
        HitRecord shadowHit = castRay(shadowRay);        
        return shadowHit.isHit() && glm::length(shadowHit.point - shadow) < max;
    }
    
    // Compute color at the hit point using the phong-multiple shader as a base.
    glm::vec3 getBase(const HitRecord& hit, const vector<util::Light*>& lights) {
        glm::vec3 matAmbient = glm::vec3(hit.material.getAmbient());
        glm::vec3 matDiffuse = glm::vec3(hit.getColor());
        glm::vec3 matSpecular = glm::vec3(hit.material.getSpecular());
        float shininess = hit.material.getShininess();
        glm::vec3 normal = glm::normalize(hit.normal);
        glm::vec3 view = glm::normalize(-hit.point);
        glm::vec3 color = glm::vec3(0.0f);
        for (const auto& light : lights) {
            if (inShadow(hit.point, normal, light)) {
                continue; 
            }
            glm::vec4 lightPos = light->getPosition();
            float spotAngle = light->getSpotCutoff();
            glm::vec3 lightVec;
            if (spotAngle != 0.0f) {
                lightVec = glm::normalize(glm::vec3(lightPos) - hit.point);
            } else {
                lightVec = glm::normalize(-glm::vec3(lightPos));
            }
            float nDotL = glm::dot(normal, lightVec);
            float spotEffect = 1.0f;
            if (spotAngle > 0.0f) {
                glm::vec3 spotDir = glm::normalize(light->getSpotDirection());
                float spotCos = glm::dot(lightVec, spotDir);
                if (spotCos >= cos(spotAngle)) {
                    spotEffect = 0.0f;
                }
            }
            glm::vec3 reflect = glm::normalize(glm::reflect(-lightVec, normal));
            float rDotV = max(glm::dot(reflect, view), 0.0f);            
            glm::vec3 ambient = matAmbient * glm::vec3(light->getAmbient()) * spotEffect;            
            glm::vec3 diffuse = matDiffuse * glm::vec3(light->getDiffuse()) * max(nDotL, 0.0f) * spotEffect;            
            glm::vec3 specular = glm::vec3(0.0f);
            if (nDotL > 0.0f) {
                specular = matSpecular * glm::vec3(light->getSpecular()) * pow(rDotV, shininess) * spotEffect;
            }
            color += ambient + diffuse + specular;
        }
        return glm::clamp(color, 0.0f, 1.0f);
    }

    //computes refraction direction using Snell's law.
    bool computeRefraction(const glm::vec3& ray, const glm::vec3& normal, 
                           float n1, float n2, glm::vec3& refracted) {
        float eta = n1 / n2;
        float cos = -glm::dot(ray, normal);
        float sin = eta * eta * (1.0f - cos * cos);
        if (sin > 1.0f) return false;
        refracted = glm::normalize(eta * ray + (eta * cos - sqrt(1.0f - sin)) * normal);
        return true;
    }

    //computes the color at the hit point including reflections and refractions.
    glm::vec3 computeColor(const HitRecord& hit, const vector<util::Light*>& lights, int bounces = 0, float curr = 1.0f) {
        if (!hit.isHit() || bounces >= 5) {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
        float absorption = hit.material.getAbsorption();
        float reflect = hit.material.getReflection();
        float transparency = hit.material.getTransparency();
        float refractIndex = hit.material.getRefractiveIndex();        
        glm::vec3 baseColor = getBase(hit, lights);        
        if (reflect == 0.0f && transparency == 0.0f) {
            return glm::clamp(baseColor, 0.0f, 1.0f);
        }
        glm::vec3 normal = glm::normalize(hit.normal);
        glm::vec3 view = glm::normalize(hit.point);
        bool enter = glm::dot(view, normal) < 0.0f;
        normal = enter ? normal : -normal;
        glm::vec3 color = baseColor * absorption;
        if (reflect > 0.0f) {
            glm::vec3 direction = glm::reflect(view, normal);        
            glm::vec3 origin = hit.point + normal * 0.001f;
            Ray reflection(origin, direction);        
            HitRecord reflectHit = castRay(reflection);        
            glm::vec3 reflectColor = computeColor(reflectHit, lights, bounces + 1, curr);  
            color +=  reflect * reflectColor;
        }      
        if (transparency > 0.0f) {
            glm::vec3 refractDir;
            float n1 = enter ? curr : refractIndex;
            float n2 = enter ? refractIndex : 1.0f;
            if (computeRefraction(view, normal, n1, n2, refractDir)) {
                glm::vec3 origin = hit.point - normal * 0.001f;
                Ray refraction(origin, refractDir);
                HitRecord refractHit = castRay(refraction);
                float next = enter ? refractIndex : 1.0f;
                color += transparency * computeColor(refractHit, lights, bounces + 1, next);
            } else {
                glm::vec3 direction = glm::reflect(view, normal);
                glm::vec3 origin = hit.point + normal * 0.001f;
                Ray reflection(origin, direction);
                HitRecord reflectHit = castRay(reflection);
                color += transparency * computeColor(reflectHit, lights, bounces + 1, curr);
            }
        }
        return glm::clamp(color, 0.0f, 1.0f);
    }
    
    //cast a ray into the scenegraph.
    HitRecord castRay(const Ray& ray) {
        renderer->setRay(ray);
        scenegraph->getRoot()->accept(renderer);
        return renderer->getClosestHit();
    }
    
    //renders the scenegraph.
    void render(const vector<util::Light*>& lights) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                Ray ray = generateRay(i, j);
                HitRecord hit = castRay(ray);
                glm::vec3 color = computeColor(hit, lights);
                int id = 3 * (j * width + i);
                image[id] = (unsigned char)(color.r * 255);
                image[id + 1] = (unsigned char)(color.g * 255);
                image[id + 2] = (unsigned char)(color.b * 255);
            }
        }
    }
    
    //saves the image to a PPM file.
    void saveImage(const string& filename) {
        FILE* fp = fopen(filename.c_str(), "w"); 
        if (!fp) {
            return;
        }
        fprintf(fp, "P3\n%d %d\n255\n", width, height);
        for (int i = 0; i < image.size(); i += 3) {
            fprintf(fp, "%d %d %d ", image[i], image[i+1], image[i+2]);
            if ((i/3 + 1) % width == 0) {
                fprintf(fp, "\n");  
            }
        }
        fclose(fp);
    }

    private:
        int width, height;
        vector<unsigned char> image;
        stack<glm::mat4>& modelview;
        sgraph::RaycastRenderer* renderer;
        sgraph::IScenegraph* scenegraph;
        float fov;
    };
}

#endif