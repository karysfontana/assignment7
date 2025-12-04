#include "KDTree.h"
#include <iostream>
#include <limits>
using namespace std;

KDTree::KDTree(util::PolygonMesh<VertexAttrib>& mesh) {
    vector<VertexAttrib> vertices = mesh.getVertexAttributes();
    vector<float> v;
    minBounds = glm::vec3(numeric_limits<float>::max(),numeric_limits<float>::max(),numeric_limits<float>::max());
    maxBounds = glm::vec3(numeric_limits<float>::min(),numeric_limits<float>::min(),numeric_limits<float>::min());
    for (int i=0;i<vertices.size();i+=1) {
        v = vertices[i].getData("position");
        glm::vec3 glmv = glm::vec3(v[0],v[1],v[2]);
        minBounds = glm::min(minBounds,glmv);
        maxBounds = glm::max(maxBounds,glmv);
        this->vertices.push_back(glmv);
        
        v = vertices[i].getData("normal");
        this->normals.push_back(glm::vec3(v[0],v[1],v[2]));
    }
    
    maxPointsPerLeaf = 5;
    root = buildKDTree(maxPointsPerLeaf);

    //now extract and add the triangles to the kdtree
    // Load primitives 
    vector<unsigned int> prim = mesh.getPrimitives(); 
    int primSize = mesh.getPrimitiveSize(); 
    // Since this is after the vertices and normals are loaded, assume triangles 
    for (int i = 0; i < prim.size(); i += primSize)
    {
        glm:ivec3 tri(
            prim[i], 
            prim[i + 1], 
            prim[i + 2]
        ); 
        triangles.push_back(tri); 
    }

    // Assign triangles to KDNodes.
    vector<int> allTri(triangles.size());
    iota(allTri.begin(), allTri.end(), 0); 
    assignTrianglesToNode(root, allTri);     
}

KDTree::~KDTree() {
    if (root!=NULL) {
        delete root;
    }
}

HitRecord KDTree::intersect(Ray& objectRay,Ray& viewRay,glm::mat4 normalMatrix) {
    HitRecord hit;
    hit.t = std::numeric_limits<float>::infinity(); 

    float tmin, tmax; 

    // IF ray misses box, skip ! 
    if (!intersect_bounding_box(objectRay, &tmin, &tmax))
        return hit; 
    
    // ELSE do kd traversal 
    return intersectNode(root, objectRay, tmin, tmax); 
}


KDNode *KDTree::buildKDTree(int maxPointsPerLeaf) {

    vector<int> sortedByX;
    vector<int> sortedByY;
    vector<int> sortedByZ;
    for (int i=0;i<vertices.size();i+=1){
        sortedByX.push_back(i);
        sortedByY.push_back(i);
        sortedByZ.push_back(i);
    }
    // Sort the vector in ascending order using x
    std::sort(sortedByX.begin(), sortedByX.end(), [this](int a, int b) {
        return vertices[a][0] < vertices[b][0]; // Returns true if 'a' should come before 'b'
    });
    // Sort the vector in ascending order using y
    std::sort(sortedByY.begin(), sortedByY.end(), [this](int a, int b) {
        return vertices[a][1] < vertices[b][1]; // Returns true if 'a' should come before 'b'
    });
    // Sort the vector in ascending order using z
    std::sort(sortedByZ.begin(), sortedByZ.end(), [this](int a, int b) {
        return vertices[a][2] < vertices[b][2]; // Returns true if 'a' should come before 'b'
    });
    return buildKDTree(sortedByX,sortedByY,sortedByZ,maxPointsPerLeaf,0); 
}

KDNode *KDTree::buildKDTree(vector<int>& sortedByX,vector<int>& sortedByY,vector<int>& sortedByZ,int maxPointsPerLeaf,int depth) {
    int splitDimension = depth % 3;
    int numPoints = sortedByX.size();

    if (numPoints == 0) {
        return NULL;
    }

    if (numPoints<=maxPointsPerLeaf) {
        return new KDLeafNode(&vertices,&normals,sortedByX,this);
    }

    glm::vec4 plane_equation;
    switch(splitDimension) {
        case 0: 
        plane_equation = glm::vec4(0.0f,0.0f,0.0f,-vertices[sortedByX[numPoints/2]][splitDimension]);
        break;
        case 1: 
        plane_equation = glm::vec4(0.0f,0.0f,0.0f,-vertices[sortedByY[numPoints/2]][splitDimension]);
        break;
        case 2: 
        plane_equation = glm::vec4(0.0f,0.0f,0.0f,-vertices[sortedByZ[numPoints/2]][splitDimension]);
        break;
    }
    plane_equation[splitDimension] = 1;

    //now split X, Y, Z lists into two, for the two halves
    vector<int> xBefore,yBefore,zBefore,xAfter,yAfter,zAfter,on;
    for (int i=0;i<sortedByX.size();i+=1) {
        float signed_distance = glm::dot(plane_equation,glm::vec4(vertices[sortedByX[i]],1.0f));
        if (signed_distance<0) { // ray's start is on the left side
            xBefore.push_back(sortedByX[i]);
        }
        else if (signed_distance>0) { 
            xAfter.push_back(sortedByX[i]);
        }
        else {
            on.push_back(sortedByX[i]);
        }
    }

    for (int i=0;i<sortedByY.size();i+=1) {
        float signed_distance = glm::dot(plane_equation,glm::vec4(vertices[sortedByY[i]],1.0f));
        if (signed_distance<0) { // ray's start is on the left side
            yBefore.push_back(sortedByY[i]);
        }
        else if (signed_distance>0) { 
            yAfter.push_back(sortedByY[i]);
        }
        
    }

    for (int i=0;i<sortedByZ.size();i+=1) {
        float signed_distance = glm::dot(plane_equation,glm::vec4(vertices[sortedByZ[i]],1.0f));
        if (signed_distance<0) { // ray's start is on the left side
            zBefore.push_back(sortedByZ[i]);
        }
        else if (signed_distance>0) { 
            zAfter.push_back(sortedByZ[i]);
        }
    }

    KDNode *left,*right;

    left = buildKDTree(xBefore,yBefore,zBefore,maxPointsPerLeaf,depth+1);
    right = buildKDTree(xAfter,yAfter,zAfter,maxPointsPerLeaf,depth+1);
    return new KDInternalNode(left,right,&vertices,&normals,on,plane_equation,this);


}

bool KDTree::intersect_bounding_box(Ray& objectRay,float *min_t,float *max_t) {
    // Make tmin and tmax variables & set to infinity so that the actual values can be found. 
    float tmin = -std::numeric_limits<float>::infinity();
    float tmax =  std::numeric_limits<float>::infinity();

    glm::vec3 origin = objectRay.origin; 
    glm::vec3 dir = objectRay.direction; 

    // Iterate for each axis 
    for (int axis = 0; axis < 3; axis++)
    {
        float invD = 1.0f / dir[axis]; 
        float t0 = (minBounds[axis] - origin[axis]) * invD; 
        float t1 = (maxBounds[axis] - origin[axis]) * invD; 

        if (invD < 0.0f)
            std::swap(t0, t1); 

        tmin = std::max(tmin, t0); 
        tmax = std::min(tmax, t1)

        // If the ray misses (o intersection). 
        if (tmax < tmin)
            return false; 
    }
    float *min_t = tmin; 
    float *max_t = tmax; 
    // Ray is between tmin and tmax when it intersects the box 
    return true;
}

/**
 * @brief Assigns triangles from list to node based on plane location. 
 * 
 * @param n node that triangles are being assigned to. 
 * @param triangleList list of triangles to assign. 
 */
void KDTree::assignTrianglesToNode(KDNode* n, const vector<int>& triangleList)
{
    if (!n) 
        return; 

    KDAbstractNode* node = dynamic_cast<KDAbstractNode>(n); 
    KDInternalNode* internal = dynamic_cast<KDInternalNode>(n); 
    KDLeafNode* leaf = dynamic_cast<KDLeafNode>(n); 

    if (!node)
        return; 

    // Leaf node will store all triangles that touch 
    if (leaf)
    {
        for (int tID : triangleList)
        {
            glm::ivec3 tri = triangles[tID]; 
            int v0 = tri.x, 
                v1 = tri.y,
                v2 = tri.z; 
            
            bool belongs = 
                find(node->indices.begin(), node->indices.end(), v0) != node->indices.end() ||
                find(node->indices.begin(), node->indices.end(), v1) != node->indices.end() ||
                find(node->indices.begin(), node->indices.end(), v2) != node->indices.end(); 
            if (belongs)
                node->triangleIndices.push_back(tID);       
        }
        return; 
    }

    // Handle internal node 
    glm::vec4 plane = internal->getPlane(); 
    vector<int> leftList, rightList, onList; 
    // group triangles relative to plane 
    for (int tID : triangleList)
    {
        auto tri = triangles[tID]; 
        glm::vec3 p0 = vertices[tri.x]; 
        glm::vec3 p1 = vertices[tri.y]; 
        glm::vec3 p2 = vertices[tri.z]; 

        float d0 = dot(plane, glm::vec4(p0, 1)); 
        float d1 = dot(plane, glm::vec4(p1, 1)); 
        float d2 = dot(plane, glm::vec4(p2, 1)); 

        bool neg = (d0 < 0) || (d1 < 0) || (d2 < 0); 
        bool pos = (d0 > 0) || (d1 > 0) || (d2 > 0); 

        if (!neg && !pos)
        {
            // Triangle lies on the plane exactly, belongs to this internal node. 
            node->triangleIndices.push_back(tID); 
        }
        else 
        {
            // Doesn't lie on the plane exactly, must belong to either left or right node. 
            if (neg)
            {
                leftList.push_back(tID); 
            }
            if (pos)
            {
                rightList.push_back(tID); 
            }
        }
    }
    // Traverse downwards recursively. 
    assignTrianglesToNode(internal->getLeft(), leftList); 
    assignTrianglesToNode(internal->getRight(), rightList); 
}
/**
 * @brief Method to check for triangle intersections with one ray tracer ray. 
 * 
 * @param ray raytracer. 
 * @param tri triangle to check for intersection with ray. 
 * @return HitRecord location of where ray intersects with triangle. 
 */

HitRecord KDTree::intersectTriangle(const Ray&ray, const glm::ivec3& tri)
{
    HitRecord hit; 
    hit.t = std::numeric_limits<float>::infinity(); 

    // Get triangle vertices 
    glm::vec3 v0 = vertices[tri.x]; 
    glm::vec3 v1 = vertices[tri.y];
    glm::vec3 v2 = vertices[tri.z]; 

    // Moller-Trumbore 
    const float EPSILON = 1e-8f; 
    glm::vec3 edge1 = v1 - v0; 
    glm::vec3 edge2 = v2 - v0; 
    glm::vec3 h = glm::cross(ray.direction, edge2); 
    float a = glm::dot(edge1, h); 

    // If ray is parallel to triangle. 
    if (fabs(a) < EPSILON)
        return hit; 

    // 'Else' ray is not parallel to triangle
    float f = 1.0f / a; 
    glm::vec3 s = ray.origin - v0; 
    u = f * glm::dot(s, h); 
    if (u < 0.0f || u > 1.0f)
        return hit; 

    glm::vec3 q = glm::cross(s, edge1); 
    float v = f * glm::dot(ray.direction, q); 
    if (v < 0.0f || u + v > 1.0f)
        return hit; 

    float t = f * glm::dot(edge2, q);
    // If ray intersects triangle
    if (t > EPSILON)
    {
        hit.t = t; 
        hit.position = ray.origin + t * ray.direction; 

        // Interpolation for vertex normals to normal 
        glm::vec3 n0 = normals[tri.x];
        glm::vec3 n1 = normals[tri.y]; 
        glm::vec3 n1 = normals[tri.z]; 
        hit.normal = glm::normalize((1 - u - v) * n0 + u * n1 + v * n2);  
    }
    return hit; 
}
/**
 * @brief Method that dertermines which sides of the tree rhe ray visits based on position relative to plane. 
 * 
 * @param node reference to node to intersect. 
 * @param ray copy of ray for intersection. 
 * @param tmin minimum value (=0). 
 * @param tmax maximum value (= infinity). 
 * @return HitRecord location of node intersection. 
 */
HitRecord KDTree::intersectNode(KDNode* node, Ray& ray, float tmin, float tmax)
{
    // Empty hit check 
    if (!node)
        return HitRecord(); 
    KDLeafNode* leaf = dynamic_cast<KDLeafNode*>(node); 
    // Test all triangles for leaf node, return closest hit. 
    if (leaf)
    {
        HitRecord closestHit; 
        closestHit.t = std::numeric_limits<float>::infinity(); 
        for (int tID : leaf->triangleIndices)
        {
            glm::ivec3 tri = triangles[tID];
            HitRecord hit = intersectTriangle(ray, tri); 
            if (hit.t >= tmin && hit.t <= tmax && hit.t < closestHit.t)
                closestHit = hit; 
        }
        return closestHit; 
    }

    KDInternalNode* internal = dynamic_cast<KDInternalNode*>(node); 
    if (!internal)
        return HitRecord(); 

    glm::vec4 plane = internal->getPlane(); 
    glm::vec3 planeNormal = glm::vec3(plane); 
    float planeD = plane.w; 

    // Math for distances of P and Q to the plane 
    glm::vec3 P = ray.origin + tmin * ray.direction; 
    glm::vec3 Q = ray.origin + tmax * ray.direction; 
    float distP = glm::dot(planeNormal, P) + planeD; 
    float distQ = glm::dot(planeNormal, Q) + planeD; 
    const float EPSILON = 1e-6f; 

    // Determine sides 
    KDNode* first = nullptr; 
    KDNode* second = nullptr; 
    if(distP > EPSILON)
    {
        first = internal->getLeft(); 
        second = internal->getRight();
    }
    else if (distP > EPSILON)
    {
        first = internal->getRight(); 
        second = internal->getLeft(); 
    }
    // If P is on plane
    else 
    {
        first = internal->getLeft(); 
        second = internal->getRight(); 
    }
    HitRecord hit; 

    // Check ray intersecting plane. 
    float denom = glm::dot(planeNormal, ray.direction); 
    float t_plane = std::numeric_limits<float>::infinity(); 
    if (fabs(denom) > EPSILON)
    {
        t_plane = -(glm::dot(planeNormal, ray.origin) + planeD) / denom; 
    }
    // If ray lies on plane (when denom = 0). 
    if (fabs(denom) < EPSILON || fabs(distP) < EPSILON && fabs(distQ) < EPSILON)
    {
        HitRecord hitLeft = intersectNode(internal->getLeft(), ray, tmin, tmax); 
        HitRecord hitRight = intersectNode(internal->getRight(), ray, tmin, tmax); 

        // Triangles lying exactly on the plane. 
        for (int tID : internal->triangleIndices)
        {
            HitRecord planeHit = intersectTriangle(ray, triangles[tID]); 
            if (planeHit.t >= tmin && planeHit.t <= tmax)
            {
                if (planeHit.t < hitLeft.t)
                    hitLeft = planeHit; 
            }
        }
        hit = (hitLeft.t < hitRight.t) ? hitLeft : hitRight
        return hit; 
    }

    // t_plane lies in (tmin, tmax) so we traverse both sides
    if (t_plane >= tmin && t_plane <= tmax)
    {
        HitRecord hitFirst = intersectNode(first, ray, tmin, t_plane); 
        HitRecord hitSecond = intersectNode(second, ray, t_plane, tmax); 

        // Triangles lying exactly on the plane. 
        for (int tID : internal->triangleIndices)
        {
            HitRecord planeHit = intersectTriangle(ray, triangles[tID]); 
            if (planeHit.t >= tmin && planeHit.t <= tmax)
            {
                if (planeHit.t < hitLeft.t)
                    hitLeft = planeHit; 
            }
        }

        hit = (hitFirst.t < hitSecond.t) ? hitFirst : hitSecond; 
    }
    // Traverse only the side P is on
    else 
    {
        hit = intersectNode(first, ray, tmin, tmax); 

        // Check plane triangles 
        if(fabs(distP) < EPSILON)
        {
            for (int tID : internal->triangleIndices)
            {
                HitRecord planeHit = intersectTriangle(ray, triangles[tID]); 
                if (planeHit.t >= tmin && planeHit.t <= tmax && planeHit.t < hit.t)
                    hit = planeHit; 
            }
        }
    }
    return hit; 
}