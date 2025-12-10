#ifndef _RAYTRACEMESH_H_
#define _RAYTRACEMESH_H_

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "HitRecord.h"
#include "Ray.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "Material.h"
using namespace std;

namespace ray {

class RaytraceMesh {
private:
    util::PolygonMesh<VertexAttrib> mesh;
    string name;
    
    // Moller–Trumbore intersection algorithm
    bool checkIntersect(const Ray& ray, 
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

    /**
     * @brief Helper method to get box UVs from hit point and interpolated normal. 
                Uses the assumption that that the box is centered at (0, 0, 0) like in example picture. 
     * 
     * @param point 
     * @param normal 
     * @param outU 
     * @param outV 
     */
    void computeBoxUV(const glm::vec3& point, const glm::vec3& normal, float& outU, float& outV)
    {
        // Convert to [0,1] range like in assignment example picture
        float x = point.x + 0.5f; 
        float y = point.y + 0.5f; 
        float z = point.z + 0.5f; 

        glm::vec3 absN = glm::abs(normal); 

        // Determine which face was hit 
        enum Face { LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK}; 
        Face face; 
        if (absN.x > absN.y && absN.x > absN.z)
            face = (normal.x > 0) ? RIGHT : LEFT; 
        else if (absN.y > absN.z)
            face = (normal.y > 0) ? TOP : BOTTOM; 
        else 
            face = (normal.z > 0) ? FRONT : BACK; 

        // Like in img. each face is 0.25 wide and high 
        const float boxw = 0.25f; 
        const float boxh = 0.25f; 

        switch (face)
        {
            case RIGHT:
                // at [0.50,0.75] x [0.25,0.50]
                outU = 0.50f + z * boxw; 
                outV = 0.25f + y * boxh; 
                break; 
            
            case LEFT: 
                // at [0.00, 0.25] x [0.25,0.50]
                outU = 0.00f + (1.0f - z) * boxw; 
                outV = 0.25f + y * boxh; 
                break;  
            
            case FRONT: 
                // at [0.75,1.00] x [0.25,0.50]
                outU = 0.75f + (1.0f - x) * boxw; 
                outV = 0.25f + y * boxh; 
                break; 
            
            case BACK: 
                // at [0.25,0.50] x [0.25,0.50]
                outU = 0.25f + x * boxw; 
                outV = 0.25f + y * boxh; 
                break; 
            
            case TOP: 
                // at [0.25,0.50] x [0.50,0.75]
                outU = 0.25f + x * boxw; 
                outV = 0.50f + z * boxh; 
                break; 
            
            case BOTTOM: 
                // at [0.25,0.50] x [0.00,0.25]
                outU = 0.25f + x * boxw; 
                outV = 0.00f + (1.0f - z) * boxh; 

        }

    }
    
public:
        RaytraceMesh(const string& name, const util::PolygonMesh<VertexAttrib>& mesh)
            : name(name), mesh(mesh) {}

        // Perform ray-mesh intersection
        HitRecord intersect(const Ray& rayObject, 
                    const Ray& rayView,
                    const glm::mat4& mv,
                    const glm::mat4& normalMatrix,
                    const util::Material& material) {
            HitRecord closestHit;
            closestHit.t = numeric_limits<float>::max();
            const vector<VertexAttrib>& vertices = mesh.getVertexAttributes();
            const vector<unsigned int>& primitives = mesh.getPrimitives();
            for (size_t i = 0; i < primitives.size(); i += 3) {
                unsigned int prim1 = primitives[i];
                unsigned int prim2 = primitives[i + 1];
                unsigned int prim3 = primitives[i + 2];
                glm::vec3 v1 = vertices[prim1].getPosition();
                glm::vec3 v2 = vertices[prim2].getPosition();
                glm::vec3 v3 = vertices[prim3].getPosition();
                float t, u, v;
                if (checkIntersect(rayObject, v1, v2, v3, t, u, v)) {
                    if (t > 0.001f) {
                        glm::vec3 point = rayObject.at(t);
                        glm::vec3 pv = glm::vec3(mv * glm::vec4(point, 1.0f));                
                        float tv = glm::length(pv - rayView.origin);
                        if (tv < closestHit.t) {
                            float w = 1.0f - u - v;

                            // for normal interpolation (not edited from before andres dw)
                            glm::vec3 n1 = vertices[prim1].getNormal();
                            glm::vec3 n2 = vertices[prim2].getNormal();
                            glm::vec3 n3 = vertices[prim3].getNormal();
                            glm::vec3 normal = w * n1 + u * n2 + v * n3;
                            glm::vec3 nv = glm::normalize(glm::vec3(normalMatrix * glm::vec4(normal, 0.0f)));
                            closestHit.setHit(tv, pv, nv, material);

                            glm::vec3 geometricNormal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
                            // UV interpolation 
                            if (name == "box" || name == "box-outside")
                            {
                                float U, V; 
                                computeBoxUV(point, geometricNormal, U, V); 
                                // store UVs in hit record 
                                closestHit.u = U; 
                                closestHit.v = V; 
                            }
                            else if (name == "sphere")
                            {
                                glm::vec3 n = normalize(point); 
                                float theta = atan2(n.z, n.x); 
                                float phi = asin(n.y); 
                                // Store UVs
                                closestHit.u = 0.5f + theta / (2.0f * M_PI); 
                                closestHit.v = 0.5f - phi / M_PI; 
                            }
                            else 
                            {
                                closestHit.u = 0.0f; 
                                closestHit.v = 0.0f; 
                            }

                        }
                    }
                }
            }
            return closestHit;
        }
    
        string getName() const { return name; }
    };

}

#endif