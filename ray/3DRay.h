#ifndef _3DRAY_H_
#define _3DRAY_H_

#include "glm/glm.hpp"

namespace ray {
    class 3DRay {
        public:
            glm::vec3 start;
            glm::vec3 direction;
    
            3DRay() : start(0.0f), direction(0.0f, 0.0f, -1.0f) {}

            3DRay(const glm::vec3& start, const glm::vec3& direction) 
                : start(start), direction(glm::normalize(direction)) {}
    
            glm::vec3 at(float t) const {
                return start + direction * t;
            }
    };
}

#endif 