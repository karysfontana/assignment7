#ifndef __KDTREE_H__
#define __KDTREE_H__

#include "KDNode.h"
#include "KDInternalNode.h"
#include "KDLeafNode.h"
#include <limits>
#include <vector>
using namespace std;
#include "VertexAttrib.h"
#include "PolygonMesh.h"
#include <glm/glm.hpp>

class KDTree {
    friend class KDInternalNode;
    friend class KDAbstractNode;
    public:
    KDTree(util::PolygonMesh<VertexAttrib>& mesh);

    ~KDTree();

    virtual HitRecord intersect(Ray& objectRay,Ray& viewRay,glm::mat4 normalMatrix);

    // Helper method to assign triangles 
    void assignTrianglesToNode(KDNode* node, const vector<int>& triangleIDs); 

    private:
    KDNode *buildKDTree(int maxPointsPerLeaf);
    KDNode *buildKDTree(vector<int>& sortedByX,vector<int>& sortedByY,vector<int>& sortedByZ,int maxPointsPerLeaf,int depth);
    bool intersect_bounding_box(Ray& objectRay,float *min_t,float *max_t);
    

    private:
    KDNode *root;
    int maxPointsPerLeaf;
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    // Was included from import 
    vector<glm::ivec3> triangles;
    glm::vec3 minBounds,maxBounds;
};

#endif