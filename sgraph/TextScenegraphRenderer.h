#ifndef _TEXTSCENEGRAPH_H_
#define _TEXTSCENEGRAPH_H_

#include "SGNodeVisitor.h"
#include "SGNode.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include <ShaderProgram.h>
#include <ShaderLocationsVault.h>
#include "ObjectInstance.h"
#include <stack>
#include <iostream>
using namespace std;

namespace sgraph {
    class TextScenegraphRenderer: public SGNodeVisitor {
    public:
        TextScenegraphRenderer() : level(0), tick(0) {}

        //prints out
        string getOutput() const {
            return out;
        }

        // Visits a group node and moves onto the children of said group node.
        // while saving the name of the group node.
        void visitGroupNode(GroupNode* node) override {
            for (int i = 0; i < level; ++i){
                out += "    ";
            }
            out += "- " + node->getName() + "\n";
            level++;
            for (SGNode* c : node->getChildren()) {
                c->accept(this);
            }
            level--;
        }

        void visitAnimationTransform(AnimationTransform *animationNode) {
            animationNode->setAnimation(tick);
            this->setAnimation(animationNode);
            visitTransformNode(animationNode);
        }

        glm::mat4 getAnimation() {
            return currAnimation;
        }

        // Visits a transform node and moves onto the children of said transform node 
        // while saving the name of the transform node.
        void visitTransformNode(TransformNode* node) override {
            for (int i = 0; i < level; ++i){
                out += "    ";
            }
            out += "- " + node->getName() + "\n";
            level++;
            for (SGNode* c : node->getChildren()) {
                c->accept(this);
            }
            level--;
        }

        // Visits a leaf node and saves the name of the node.
        void visitLeafNode(LeafNode* node) override {
            for (int i = 0; i < level; ++i){
                out += "    ";
            }
            out += "- " + node->getName() + "\n";
        }

        // visits a scale node and sends to transform node.
        void visitScaleTransform(ScaleTransform *scaleNode) {
            visitTransformNode(scaleNode);
        }

        // visits a translate node and sends to transform node.
        void visitTranslateTransform(TranslateTransform *translateNode) {
            visitTransformNode(translateNode);
        }

        // visits a rotation node and sends to transform node.
        void visitRotateTransform(RotateTransform *rotateNode) {
            visitTransformNode(rotateNode);
        }

        void setTick(int t) {
            this->tick = t;
        }

        void setShaderLocations(const util::ShaderLocationsVault &locations) {
            this->shaderLocations = locations;
        }

        void setTextureMap(map<string,unsigned int> textureIds) {
            ids = textureIds;
        }

        private:
            //out is the output that is printed
            //level is the current amount of indents for the text output.
            string out;
            int level;
            int tick;
            vector<util::Light> lights;
            util::ShaderLocationsVault shaderLocations;
            map<string,unsigned int> ids;
            glm::mat4 currAnimation;

            void setAnimation(AnimationTransform *animationNode) {
                currAnimation = animationNode->getTransform();
            }
   };
}

#endif