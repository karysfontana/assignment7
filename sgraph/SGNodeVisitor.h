#ifndef _SGNODEVISITOR_H_
#define _SGNODEVISITOR_H_

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Light.h"
using namespace std;

/*
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "ScaleTransform.h"
#include "RotateTransform.h"
#include "TranslateTransform.h"
#include "AnimationTransform.h"
*/

namespace sgraph {
    class GroupNode;
    class LeafNode;
    class TransformNode;
    class ScaleTransform;
    class RotateTransform;
    class TranslateTransform;
    class AnimationTransform;

/**
 * This class represents the interface for a visitor on scene graph nodes.
 * The visitor design pattern ensures that one can implement various 
 * functions on the scene graph nodes, while not having to change the nodes.
 * 
 * 
 */
    class SGNodeVisitor {
        public:
        virtual void visitGroupNode(GroupNode *node)=0;
        virtual void visitLeafNode(LeafNode *node)=0;
        virtual void visitTransformNode(TransformNode *node)=0;
        virtual void visitScaleTransform(ScaleTransform *node)=0;
        virtual void visitTranslateTransform(TranslateTransform *node)=0;
        virtual void visitRotateTransform(RotateTransform *node)=0;
        virtual void visitAnimationTransform(AnimationTransform *node)=0;
        virtual glm::mat4 getAnimation()=0;
        virtual void setTick(int t)=0;
        virtual void setShaderLocations(const util::ShaderLocationsVault &locations)=0;
        virtual void setTextureMap(map<string,unsigned int> textureIds)=0;
        
    };
}

#endif