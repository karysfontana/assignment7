#ifndef _RAYTRACEMESH_H_
#define _RAYTRACEMESH_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "RaycastingHelpers.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "HitRecord.h"
#include "Material.h"

class RaytraceMesh {
private:
    PolygonMesh<VertexAttrib> mesh;
    std::string name;
    
    bool intersectTriangle(const Ray& ray, 
                          const glm::vec3& v0, 
                          const glm::vec3& v1, 
                          const glm::vec3& v2,
                          float& t, float& u, float& v) {
        const float EPSILON = 0.0000001f;
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 h = glm::cross(ray.direction, edge2);
        float a = glm::dot(edge1, h);
        if (a > -EPSILON && a < EPSILON) {
            return false; 
        }
        float f = 1.0f / a;
        glm::vec3 s = ray.origin - v0;
        u = f * glm::dot(s, h);
        if (u < 0.0f || u > 1.0f) {
            return false;
        }
        glm::vec3 q = glm::cross(s, edge1);
        v = f * glm::dot(ray.direction, q);
        if (v < 0.0f || u + v > 1.0f) {
            return false;
        }
        t = f * glm::dot(edge2, q);
        if (t > EPSILON) {
            return true; 
        }
        return false; 
    }
    
public:
    RaytraceMesh(const std::string& name, const PolygonMesh<VertexAttrib>& mesh)
        : name(name), mesh(mesh) {}

    HitRecord intersect(const Ray& rayObject, 
                       const Ray& rayView,
                       const glm::mat4& modelview,
                       const glm::mat4& normalMatrix) {
        HitRecord closestHit;
        closestHit.t = std::numeric_limits<float>::max();
        const std::vector<VertexAttrib>& vertices = mesh.getVertexAttributes();
        const std::vector<unsigned int>& primitives = mesh.getPrimitives();
        int primitiveType = mesh.getPrimitiveType();
        int primitiveSize = mesh.getPrimitiveSize();
        if (primitiveType != GL_TRIANGLES || primitiveSize != 3) {
            return closestHit;
        }    
        for (size_t i = 0; i < primitives.size(); i += 3) {
            unsigned int idx0 = primitives[i];
            unsigned int idx1 = primitives[i + 1];
            unsigned int idx2 = primitives[i + 2];
            glm::vec3 v0 = vertices[idx0].getPosition();
            glm::vec3 v1 = vertices[idx1].getPosition();
            glm::vec3 v2 = vertices[idx2].getPosition();
            float t, u, v;
            if (intersectTriangle(rayObject, v0, v1, v2, t, u, v)) {
                if (t > 0.001f && t < closestHit.t) {
                    float w = 1.0f - u - v;
                    glm::vec3 n0 = vertices[idx0].getNormal();
                    glm::vec3 n1 = vertices[idx1].getNormal();
                    glm::vec3 n2 = vertices[idx2].getNormal();
                    glm::vec3 normalObj = w * n0 + u * n1 + v * n2;
                    glm::vec3 pointObj = rayObject.at(t);
                    glm::vec3 pointView = glm::vec3(modelview * glm::vec4(pointObj, 1.0f));
                    glm::vec3 normalView = glm::normalize(glm::vec3(normalMatrix * glm::vec4(normalObj, 0.0f)));
                    Material mat; 
                    closestHit.setHit(t, pointView, normalView, mat);
                }
            }
        }
        return closestHit;
    }
    
    std::string getName() const { return name; }
};

#endif 