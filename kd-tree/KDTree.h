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
#include "../ray/Ray.h"
#include "../ray/HitRecord.h"

class KDTree {
    friend class KDInternalNode;
    friend class KDAbstractNode;
    friend class KDLeafNode;
    public:
    KDTree(util::PolygonMesh<VertexAttrib>& mesh);

    ~KDTree();

    virtual ray::HitRecord intersect(ray::Ray& objectRay, ray::Ray& viewRay,glm::mat4 normalMatrix);

    // Helper method to assign triangles 
    void assignTrianglesToNode(KDNode* node, const vector<int>& triangleIDs); 

    private:
    KDNode *buildKDTree(int maxPointsPerLeaf);
    KDNode *buildKDTree(vector<int>& sortedByX,vector<int>& sortedByY,vector<int>& sortedByZ,int maxPointsPerLeaf,int depth);
    bool intersect_bounding_box(ray::Ray& objectRay,float *min_t,float *max_t);
    // Helper method to check for intersection with one ray. 
    ray::HitRecord intersectTriangle(const ray::Ray& ray, const glm::ivec3& tri); 
    // Recursive traversal for intersections of internal nodes. 
    ray::HitRecord intersectNode(KDNode* node, const ray::Ray& ray, float tmin, float tmax); 
    

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