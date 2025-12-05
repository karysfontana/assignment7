#ifndef _RAY_H_
#define _RAY_H_

#include "glm/glm.hpp"

namespace ray {
    class Ray {
        public:
            glm::vec3 origin;
            glm::vec3 direction;
    
            Ray() : origin(0.0f), direction(0.0f, 0.0f, -1.0f) {}

            Ray(const glm::vec3& origin, const glm::vec3& direction) 
                : origin(origin), direction(glm::normalize(direction)) {}
    
            // Gets point along the ray.
            glm::vec3 at(float t) const {
                return origin + direction * t;
            }
    };
}

#endif