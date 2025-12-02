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

namespace ray {
class Raytracer {
    public:
    Raytracer(int w, int h, 
              std::stack<glm::mat4>& mv,
              std::map<std::string, util::PolygonMesh<VertexAttrib>>& meshes,
              sgraph::IScenegraph* sg)
        : width(w), height(h), modelview(mv), scenegraph(sg), fov(60.0f) {
        imageData.resize(width * height * 3);
        renderer = new sgraph::RaycastRenderer(mv, meshes);
    }
    
    ~Raytracer() {
        delete renderer;
    }
    
    void setTextureImages(std::map<std::string, util::TextureImage*> texImages) {
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
    
    glm::vec3 computeColor(const HitRecord& hit, const std::vector<util::Light*>& lights) {
        if (!hit.isHit()) {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
        glm::vec4 ambient = hit.material.getAmbient();
        glm::vec4 diffuse = hit.getColor();
        glm::vec4 specular = hit.material.getSpecular();
        float shininess = hit.material.getShininess();
        glm::vec3 color = glm::vec3(ambient);        
        for (const auto& light : lights) {
            glm::vec4 lightPos = light->getPosition();
            glm::vec3 lightDir;
            if (lightPos.w == 0.0f) {
                lightDir = glm::normalize(glm::vec3(lightPos));
            } else {
                lightDir = glm::normalize(glm::vec3(lightPos) - hit.point);
            }
            float diffuseFactor = std::max(glm::dot(hit.normal, lightDir), 0.0f);
            color += diffuseFactor * glm::vec3(diffuse) * glm::vec3(light->getDiffuse());
            if (diffuseFactor > 0.0f) {
                glm::vec3 viewDir = glm::normalize(-hit.point);
                glm::vec3 reflectDir = glm::reflect(-lightDir, hit.normal);
                float specFactor = pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), shininess);
                color += specFactor * glm::vec3(specular) * glm::vec3(light->getSpecular());
            }
        }
        return glm::clamp(color, 0.0f, 1.0f);
    }
    
    HitRecord castRay(const Ray& ray) {
        renderer->setRay(ray);
        scenegraph->getRoot()->accept(renderer);
        return renderer->getClosestHit();
    }
    
    void render(const std::vector<util::Light*>& lights) {
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
    
    void saveImage(const std::string& filename) {
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
        std::vector<unsigned char> imageData;
        std::stack<glm::mat4>& modelview;
        sgraph::RaycastRenderer* renderer;
        sgraph::IScenegraph* scenegraph;
        float fov;
};

}

#endif