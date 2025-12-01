#ifndef _GLLIGHTRENDERER_H_
#define _GLLIGHTRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "ScaleTransform.h"
#include "RotateTransform.h"
#include "TranslateTransform.h"
#include "AnimationTransform.h"

#include <stack>
#include <vector>
using namespace std;

namespace sgraph {

class GLLightRenderer : public SGNodeVisitor {
public:
    GLLightRenderer(stack<glm::mat4>& mv, util::ShaderLocationsVault& shaderLocations)
        : modelview(mv) {
            this->shaderLocations = shaderLocations;
        }

    void visitGroupNode(GroupNode *node) {
        getLights(node);
        for (SGNode *child : node->getChildren())
            child->accept(this);
    }

    void visitLeafNode(LeafNode *node) {
        getLights(node);
    }

    void visitTransformNode(TransformNode *node) {
        modelview.push(modelview.top());
        modelview.top() *= node->getTransform();
        getLights(node);
        for (SGNode *child : node->getChildren())
            child->accept(this);
        modelview.pop();
    }

    void visitScaleTransform(ScaleTransform *node) { visitTransformNode(node); }
    void visitTranslateTransform(TranslateTransform *node) { visitTransformNode(node); }
    void visitRotateTransform(RotateTransform *node) { visitTransformNode(node); }

    void visitAnimationTransform(AnimationTransform *node) {
        node->setAnimation(tick);
        visitTransformNode(node);
    }

    glm::mat4 getAnimation()  { return glm::mat4(1.0f); }
    void setTick(int t) { tick = t; }

    void setLights() {
        int count = 0;
        for (util::Light &light : allLights) {
            string base = "light[" + to_string(count) + "]";
            glUniform3fv(shaderLocations.getLocation(base + ".ambient"),
                         1, glm::value_ptr(light.getAmbient()));
            glUniform3fv(shaderLocations.getLocation(base + ".diffuse"),
                         1, glm::value_ptr(light.getDiffuse()));
            glUniform3fv(shaderLocations.getLocation(base + ".specular"),
                         1, glm::value_ptr(light.getSpecular()));
            glUniform4fv(shaderLocations.getLocation(base + ".position"),
                         1, glm::value_ptr(light.getPosition()));
            glUniform3fv(shaderLocations.getLocation(base + ".spotDirection"),
                         1, glm::value_ptr(light.getSpotDirection()));
            glUniform1f(shaderLocations.getLocation(base + ".spotAngle"),
                        light.getSpotCutoff());
            count++;
        }
        glUniform1i(shaderLocations.getLocation("numLights"), count);
    }

    void setShaderLocations(const util::ShaderLocationsVault &s) override {
        shaderLocations = s;
    }
    void setTextureMap(std::map<std::string, unsigned int>) override {}

private:

    stack<glm::mat4>& modelview;
    vector<util::Light> allLights;
    util::ShaderLocationsVault shaderLocations;
    int tick = 0;

    void getLights(SGNode *node) {
        for (const util::Light &L : node->getLights()) {
            util::Light newLight = L;
            if (L.getSpotCutoff() != 0) {
                glm::vec4 pos = modelview.top() * L.getPosition();
                newLight.setDirection(pos.x, pos.y, pos.z);
                glm::vec3 dir = glm::mat3(modelview.top()) * L.getSpotDirection();
                newLight.setSpotDirection(dir.x, dir.y, dir.z);
            } else {
                glm::vec4 pos = modelview.top() * L.getPosition();
                newLight.setPosition(pos.x, pos.y, pos.z);
            }
            allLights.push_back(newLight);
        }
    }
};
}

#endif
