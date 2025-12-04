#ifndef _RAYCASTRENDERER_H_
#define _RAYCASTRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "ScaleTransform.h"
#include "RotateTransform.h"
#include "TranslateTransform.h"
#include "AnimationTransform.h"
#include "../ray/RaytraceMesh.h"
#include "../ray/HitRecord.h"
#include "../ray/Ray.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include <stack>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
using namespace std;

namespace sgraph {
/**
 * Raycaster renderer that traverses the scene graph and performs ray-triangle
 * intersection tests. Follows the visitor pattern like GLScenegraphRenderer.
 */
class RaycastRenderer : public SGNodeVisitor {
public:
    RaycastRenderer(stack<glm::mat4>& mv,
                   map<string, util::PolygonMesh<VertexAttrib>>& meshes)
        : modelview(mv), tick(0), animationTransform(glm::mat4(1.0f)) {
        for (auto& pair : meshes) {
            raytraceMeshes[pair.first] = new ray::RaytraceMesh(pair.first, pair.second);
        }
    }
    
    ~RaycastRenderer() {
        for (auto& pair : raytraceMeshes) {
            delete pair.second;
        }
    }
    
    void setRay(const ray::Ray& ray) {
        current = ray;
        closest.clear();
    }
    
    ray::HitRecord getClosestHit() const {
        return closest;
    }
        
    void visitGroupNode(GroupNode *groupNode) {
        for (size_t i = 0; i < groupNode->getChildren().size(); i++) {
            groupNode->getChildren()[i]->accept(this);
        }
    }
    
    void visitLeafNode(LeafNode *node) {
        string meshName = node->getInstanceOf();
        if (raytraceMeshes.find(meshName) == raytraceMeshes.end()) {
            return;
        }
        glm::mat4 inverse = glm::inverse(modelview.top());
        glm::mat4 normal = glm::transpose(inverse);
        glm::vec3 origin = glm::vec3(inverse * glm::vec4(current.origin, 1.0f));
        glm::vec3 direction = glm::normalize(glm::vec3(inverse * glm::vec4(current.direction, 0.0f)));
        ray::Ray newRay(origin, direction);
        ray::HitRecord hit = raytraceMeshes[meshName]->intersect(
            newRay, 
            current, 
            modelview.top(), 
            normal,
            node->getMaterial()
        );
        if (hit.isHit() && hit.t < closest.t) {
            closest = hit;
        }
    }
    
    void visitTransformNode(TransformNode *transformNode) {
        modelview.push(modelview.top());
        modelview.top() = modelview.top() * transformNode->getTransform();
        if (transformNode->getChildren().size() > 0) {
            transformNode->getChildren()[0]->accept(this);
        }
        modelview.pop();
    }
    
    void visitScaleTransform(ScaleTransform *scaleNode) {
        visitTransformNode(scaleNode);
    }
    
    void visitTranslateTransform(TranslateTransform *translateNode) {
        visitTransformNode(translateNode);
    }
    
    void visitRotateTransform(RotateTransform *rotationNode) {
        visitTransformNode(rotationNode);
    }
    
    void visitAnimationTransform(AnimationTransform *animationNode) {
        animationNode->setAnimation(tick);
        animationTransform = animationNode->getTransform();
        visitTransformNode(animationNode);
    }
    
    glm::mat4 getAnimation() {
        return animationTransform;
    }
    
    void setTick(int t) {
        tick = t;
    }
    
    void setShaderLocations(const util::ShaderLocationsVault &locations) {
        shaderLocations = locations;
    }
    
    void setTextureMap(const map<string,unsigned int> textureIds) {
        ids = textureIds;
    }
    
private:
    stack<glm::mat4>& modelview;
    ray::Ray current;
    ray::HitRecord closest;
    map<string, ray::RaytraceMesh*> raytraceMeshes;
    map<string, unsigned int> ids;
    int tick;
    glm::mat4 animationTransform;
    util::ShaderLocationsVault shaderLocations;
};
}

#endif