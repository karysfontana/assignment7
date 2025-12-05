#ifndef _HITRECORD_H_
#define _HITRECORD_H_

#include <glm/glm.hpp>
#include "Material.h"
#include <limits>
using namespace std;

namespace ray {
class HitRecord {
public:
    float t;                                 
    glm::vec3 point;                          
    glm::vec3 normal;                         
    util::Material material;                        
    bool hit;                      
    
    HitRecord() : t(numeric_limits<float>::max()), hit(false) {}
    
    // Set hit information
    void setHit(float t, const glm::vec3& point, const glm::vec3& normal, 
                const util::Material& material) {
        this->t = t;
        this->point = point;
        this->normal = glm::normalize(normal);
        this->material = material;
        this->hit = true;
    }
    
    // Check if there was a hit
    bool isHit() const { return hit; }
    
    // Reset the hit record
    void clear() {
        hit = false;
        t = numeric_limits<float>::max();
    }
    
    // Get the color at the hit point
    glm::vec4 getColor() const {
        return material.getDiffuse();
    }
};

}

#endif