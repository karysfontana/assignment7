#ifndef _HITRECORD_H
#define _HITRECORD_H

#include <glm/glm.hpp>
#include "Material.h"
#include "TextureImage.h"

namespace ray {
class HitRecord {
public:
    float t;                                 
    glm::vec3 point;                          
    glm::vec3 normal;                         
    Material material;                        
    bool hit;                      
    
    HitRecord() : t(std::numeric_limits<float>::max()), hasUV(false), hit(false) {}
    
    void setHit(float t, const glm::vec3& point, const glm::vec3& normal, 
                const Material& material) {
        this->t = t;
        this->point = point;
        this->normal = glm::normalize(normal);
        this->material = material;
        this->hit = true;
    }
    
    bool isHit() const { return hit; }
    
    void clear() {
        hit = false;
        t = std::numeric_limits<float>::max();
    }
    
    glm::vec4 getColor() const {
        return material.getDiffuse();
    }
};

}