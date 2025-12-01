#ifndef _RAYTRACER_H_
#define _RAYTRACER_H_

#include "HitRecord.h"
#include "3DRay.h"
#include "RaycastRenderer.h"
#include "IScenegraph.h"

namespace ray {
class Raytracer {
    public:
    Raytracer(int w, int h, 
              std::stack<glm::mat4>& mv,
              std::map<std::string, util::PolygonMesh<VertexAttrib>>& meshes,
              IScenegraph* sg)
        : width(w), height(h), modelview(mv), scenegraph(sg), fov(60.0f) {
        imageData.resize(width * height * 3);
        renderer = new RaycastRenderer(mv, meshes);
    }
    
    ~Raytracer() {
        delete renderer;
    }
    
    void setTextureImages(std::map<std::string, util::TextureImage*> texImages) {
        renderer->setTextureImages(texImages);
    }
    
    util::Ray generateRay(int i, int j) {
        float aspect = (float)width / height;
        float tanFov = tan(glm::radians(fov / 2.0f));        
        float x = (2.0f * (i + 0.5f) / width - 1.0f) * aspect * tanFov;
        float y = (1.0f - 2.0f * (j + 0.5f) / height) * tanFov;
        glm::vec3 origin(0.0f, 0.0f, 0.0f);
        glm::vec3 direction = glm::normalize(glm::vec3(x, y, -1.0f));
        return util::Ray(origin, direction);
    }
    
    glm::vec3 computeColor(const util::HitRecord& hit, const std::vector<util::Light*>& lights) {
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
    
    util::HitRecord castRay(const util::Ray& ray) {
        renderer->setRay(ray);
        scenegraph->getRoot()->accept(renderer);
        return scenegraph->getRoot()->raycast(ray, modelview.top());
    }
    
    void render(const std::vector<util::Light*>& lights) {
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                util::Ray ray = generateRay(i, j);
                util::HitRecord hit = castRay(ray);
                glm::vec3 color = computeColor(hit, lights);
                int idx = 3 * (j * width + i);
                imageData[idx + 0] = (unsigned char)(color.r * 255);
                imageData[idx + 1] = (unsigned char)(color.g * 255);
                imageData[idx + 2] = (unsigned char)(color.b * 255);
            }
        }
    }
    
    void saveImage(const std::string& filename) {
        FILE* fp = fopen(filename.c_str(), "wb");
        if (!fp) {
            return;
        }
        fprintf(fp, "P3\n%d %d\n255\n", width, height);
        fwrite(imageData.data(), 1, imageData.size(), fp);
        fclose(fp);
    }

    private:
        int width, height;
        std::vector<unsigned char> imageData;
        std::stack<glm::mat4>& modelview;
        RaycastRenderer* renderer;
        IScenegraph* scenegraph;
        float fov;
};

}

#endif