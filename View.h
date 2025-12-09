#ifndef __VIEW_H__
#define __VIEW_H__

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <ShaderProgram.h>
#include "sgraph/SGNodeVisitor.h"
#include "ObjectInstance.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "ray/Raytracer.h"
#include "sgraph/IScenegraph.h"

#include <stack>
using namespace std;


class View
{
public:
    View();
    ~View();
    void init(map<string,util::PolygonMesh<VertexAttrib>>& meshes, sgraph::IScenegraph *scenegraph);
    void display(sgraph::IScenegraph *scenegraph);
    bool shouldWindowClose();
    void closeWindow();
    void reshape(int width, int height);
    void dispose();
    void onkey(int key, int scancode, int action, int mods, glm::vec3 look, glm::vec3 target);
    void error_callback(int error, const char* description);

private: 

    GLFWwindow* window;
    util::ShaderProgram program;
    util::ShaderProgram toonProgram;
    util::ShaderLocationsVault shaderLocations;
    util::ShaderLocationsVault toonShaders;
    map<string,util::ObjectInstance *> objects;
    ray::Raytracer *raytracer;
    glm::mat4 projection;
    stack<glm::mat4> modelview;
    sgraph::SGNodeVisitor *renderer;
    int frames;
    double time;
};

#endif