#ifndef __KDLEAFNODE_H__
#define __KDLEAFNODE_H__

#include "KDAbstractNode.h"
#include <vector>
using namespace std;

class KDTree;

class KDLeafNode: public KDAbstractNode {
    friend class KDTree;
    public:
    KDLeafNode(vector<glm::vec3> *listOfPoints,vector<glm::vec3> *listOfNormals,vector<int> indices,KDTree *t)
    :KDAbstractNode(listOfPoints,listOfNormals,indices,t) {

    }

    

    


};

#endif