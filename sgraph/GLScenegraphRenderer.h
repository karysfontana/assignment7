#ifndef _GLSCENEGRAPHRENDERER_H_
#define _GLSCENEGRAPHRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include "AnimationTransform.h"
#include "TextureImage.h"
#include <ShaderProgram.h>
#include <ShaderLocationsVault.h>
#include "ObjectInstance.h"
#include <stack>
#include <iostream>
using namespace std;

namespace sgraph {
    /**
     * This visitor implements drawing the scene graph using OpenGL
     * 
     */
    class GLScenegraphRenderer: public SGNodeVisitor {
        public:
        /**
         * @brief Construct a new GLScenegraphRenderer object
         * 
         * @param mv a reference to modelview stack that will be used while rendering
         * @param os the map of ObjectInstance objects
         * @param shaderLocations the shader locations for the program used to render
         */
        GLScenegraphRenderer(stack<glm::mat4>& mv,map<string,util::ObjectInstance *>& os,util::ShaderLocationsVault& shaderLocations) 
            : modelview(mv)
            , objects(os) {
            this->shaderLocations = shaderLocations;
            this->tick = 0;
        }

        /**
         * @brief Recur to the children for drawing
         * 
         * @param groupNode 
         */
        void visitGroupNode(GroupNode *groupNode) {
            for (int i=0;i<groupNode->getChildren().size();i=i+1) {
                groupNode->getChildren()[i]->accept(this);
            }
        }

        /**
         * @brief Draw the instance for the leaf, after passing the 
         * modelview and color to the shader
         * 
         * @param leafNode 
         */
        void visitLeafNode(LeafNode *leafNode) {
            glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview.top()));
            util::Material mat = leafNode->getMaterial();
            glUniform3fv(shaderLocations.getLocation("material.ambient"), 1, glm::value_ptr(mat.getAmbient()));
            glUniform3fv(shaderLocations.getLocation("material.diffuse"), 1, glm::value_ptr(mat.getDiffuse()));
            glUniform3fv(shaderLocations.getLocation("material.specular"), 1, glm::value_ptr(mat.getSpecular()));
            glUniform1f(shaderLocations.getLocation("material.shininess"), mat.getShininess());
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(shaderLocations.getLocation("image"), 0); 
            if (ids.find(leafNode->getTexture()) != ids.end()) {
                glBindTexture(GL_TEXTURE_2D, ids[leafNode->getTexture()]);
            } else {
                glBindTexture(GL_TEXTURE_2D, ids["white"]);
            }
            objects[leafNode->getInstanceOf()]->draw();
        }

        //Uses the tick to calcualte the transformation, saves it, and sends to transform node.
        void visitAnimationTransform(AnimationTransform *animationNode) {
            animationNode->setAnimation(tick);
            this->setAnimation(animationNode);
            visitTransformNode(animationNode);
        }

        //gets the animation for camera mode 4.
        glm::mat4 getAnimation() {
            return this->currAnimation;
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         * 
         * @param transformNode 
         */
        void visitTransformNode(TransformNode * transformNode) {
            modelview.push(modelview.top());
            modelview.top() = modelview.top() * transformNode->getTransform();
            if (transformNode->getChildren().size()>0) {
                transformNode->getChildren()[0]->accept(this);
            }
            modelview.pop();
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitTransformNode above
         * 
         * @param scaleNode 
         */
        void visitScaleTransform(ScaleTransform *scaleNode) {
            visitTransformNode(scaleNode);
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitTransformNode above
         * 
         * @param translateNode 
         */
        void visitTranslateTransform(TranslateTransform *translateNode) {
            visitTransformNode(translateNode);
        }

        void visitRotateTransform(RotateTransform *rotateNode) {
            visitTransformNode(rotateNode);
        }

        //Sets the tick of the animation.
        void setTick(int t) {
            this->tick = t;
        }

        void setShaderLocations(const util::ShaderLocationsVault &locations) {
            this->shaderLocations = locations;
        }

        void setTextureMap(const map<string,unsigned int> textureIds) {
            ids = textureIds;
        }

        private:
            stack<glm::mat4>& modelview;    
            util::ShaderLocationsVault shaderLocations;
            map<string,util::ObjectInstance *> objects;
            map<string,unsigned int> ids;
            glm::mat4 currAnimation;
            int tick;

            //Sets the animation for get Animation.
            void setAnimation(AnimationTransform *animationNode) {
                currAnimation = animationNode->getTransform();
            }
   };
}

#endif