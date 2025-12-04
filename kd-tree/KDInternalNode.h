#ifndef __KDINTERNALNODE_H__
#define __KDINTERNALNODE_H__

#include "KDAbstractNode.h"


class KDInternalNode:public KDAbstractNode {
    public:
        KDInternalNode(KDNode *left,KDNode *right,vector<glm::vec3> *listOfPoints,vector<glm::vec3> *listOfNormals,vector<int> indices,glm::vec4 plane_equation,KDTree *t);
        virtual ~KDInternalNode();
        // Public getters that returns references to the left and right nodes and plane. 
        KDNode* getLeft() const { return left; }
        KDNode* getRight() const {return right; }
        glm::vec4 getPlane() const { return plane; }
        
    private:
     KDNode *left,*right;
     const glm::vec4 plane; //equation of the dividing plane

};

#endif