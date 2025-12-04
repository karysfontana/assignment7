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
        imageData.resize(width * height * 3);
        renderer = new sgraph::RaycastRenderer(mv, meshes);
    }
    
    ~Raytracer() {
        delete renderer;
    }
    
    void setTextureImages(map<string, util::TextureImage*> texImages) {
        renderer->setTextureImages(texImages);
    }
    
    Ray generateRay(int i, int j) {
        float aspect = (float)width / height;
        float tanFov = tan(glm::radians(fov / 2.0f));        
        float x = (2.0f * (i + 0.5f) / width - 1.0f) * aspect * tanFov;
        float y = (1.0f - 2.0f * (j + 0.5f) / height) * tanFov;
        glm::vec3 origin(0.0f, 0.0f, 0.0f);
        glm::vec3 direction = glm::normalize(glm::vec3(x, y, -1.0f));
        return Ray(origin, direction);
    }

    bool inShadow(const glm::vec3& point, const glm::vec3& normal, const util::Light* light) {
        glm::vec3 direction;
        float max;
        if (light->getPosition().w == 1.0f) {
            glm::vec3 position = glm::vec3(light->getPosition()) - point;
            max = glm::length(position);    
            direction = position / max;
        } else {
            direction = glm::normalize(-glm::vec3(light->getPosition()));
            max = std::numeric_limits<float>::max();
        }        
        glm::vec3 shadow = point + normal * 0.1f;
        Ray shadowRay(shadow, direction);
        HitRecord shadowHit = castRay(shadowRay);        
        return shadowHit.isHit() && glm::length(shadowHit.point - shadow) < max;
    }
    
    glm::vec3 computeColor(const HitRecord& hit, const vector<util::Light*>& lights) {
        if (!hit.isHit()) {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
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
    
    HitRecord castRay(const Ray& ray) {
        renderer->setRay(ray);
        scenegraph->getRoot()->accept(renderer);
        return renderer->getClosestHit();
    }
    
    void render(const vector<util::Light*>& lights) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                Ray ray = generateRay(i, j);
                HitRecord hit = castRay(ray);
                glm::vec3 color = computeColor(hit, lights);
                int idx = 3 * (j * width + i);
                imageData[idx + 0] = (unsigned char)(color.r * 255);
                imageData[idx + 1] = (unsigned char)(color.g * 255);
                imageData[idx + 2] = (unsigned char)(color.b * 255);
            }
        }
    }
    
    void saveImage(const string& filename) {
        FILE* fp = fopen(filename.c_str(), "w"); 
        if (!fp) {
            return;
        }
        fprintf(fp, "P3\n%d %d\n255\n", width, height);
        for (int i = 0; i < imageData.size(); i += 3) {
            fprintf(fp, "%d %d %d ", imageData[i], imageData[i+1], imageData[i+2]);
            if ((i/3 + 1) % width == 0) {
                fprintf(fp, "\n");  
            }
        }
        fclose(fp);
    }

    private:
        int width, height;
        vector<unsigned char> imageData;
        stack<glm::mat4>& modelview;
        sgraph::RaycastRenderer* renderer;
        sgraph::IScenegraph* scenegraph;
        float fov;
};

}

#endif