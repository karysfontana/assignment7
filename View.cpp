#include "View.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgraph/GLScenegraphRenderer.h"
#include "sgraph/TextScenegraphRenderer.h"
#include "sgraph/GLLightRenderer.h"
#include "VertexAttrib.h"
#include "Light.h"

// These global variables are used to determine the camera
// When 1 is pressed, you look from the stationaryLook and look at the stationaryTarget
// look and target is where the camera currently is oriented
// active2, active3, active4 determines what camera mode you are currently in
// first to manage when the text scengraph renderer prints.
// tick is used for animation and active3 camera mode to determine the rate of movement.
glm::vec3 stationaryLook = glm::vec3(0.0f,0.0f,20.0f);
glm::vec3 stationaryTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 look = stationaryLook;
glm::vec3 target = stationaryTarget;
map<string,unsigned int> textureIds;
bool active2 = false;
bool active3 = false;
bool active4 = false;
bool first = true;
bool raycast = false;
int tick = 0;

View::View() {}

View::~View() {}

// handles the camera movement mode.
// tspeed and rspeed deal with the move speed of the camera for translation and rotation.
// we calculate forward, right and up to get the current orientation of the camera.
// if mod shift is active, we use rotate to calculate and rotate around look to change the angle you are looking at.
// without shift, you can just translate the look and target to see where you are.
void handleControlled(int key, int mods) {
    float tspeed = 0.5f;
    float rspeed = 0.05f;
    glm::vec3 forward = glm::normalize(target - look);
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));
    if(mods && GLFW_MOD_SHIFT) {
        switch(key) {
            case GLFW_KEY_LEFT:
                target = look + glm::vec3(glm::rotate(glm::mat4(1.0f), rspeed, up) * glm::vec4(forward, 0.0f));
                break;
            case GLFW_KEY_RIGHT:
                target = look + glm::vec3(glm::rotate(glm::mat4(1.0f), -rspeed, up) * glm::vec4(forward, 0.0f));
                break;
            case GLFW_KEY_UP:
                target = look + glm::vec3(glm::rotate(glm::mat4(1.0f), rspeed, right) * glm::vec4(forward, 0.0f));
                break;
            case GLFW_KEY_DOWN:
                target = look + glm::vec3(glm::rotate(glm::mat4(1.0f), -rspeed, right) * glm::vec4(forward, 0.0f));
                break;
        }
    } else {
        switch (key) {
            case GLFW_KEY_LEFT:
                look -= right * tspeed;
                target -= right * tspeed;
                break;
            case GLFW_KEY_RIGHT:
                look += right * tspeed;
                target += right * tspeed;
                break;
            case GLFW_KEY_UP:
                look += up * tspeed;
                target += up * tspeed;
                break;
            case GLFW_KEY_DOWN:
                look -= up * tspeed;
                target -= up * tspeed;
                break;
            case GLFW_KEY_F:
                look += forward * tspeed;
                target += forward * tspeed;
                break;
            case GLFW_KEY_B:
                look -= forward * tspeed;
                target -= forward * tspeed;
                break;
            default:
                break;
        }
    }

}

// handles active mode 3, with rotation.
// rotates around the Y axis looking at the center of the model while rotating about an off-center axis
void handleChopper() {
    if (active3) {
        float rspeed = 0.02f;
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(35.0f, 0.0f, -35.0f)) 
            * glm::rotate(glm::mat4(1.0f), rspeed * tick, glm::vec3(0.0f, 1.0f, 0.0f));
        look = transform * glm::vec4(150.0f, 150.0f, 150.0f, 1.0f);
        target = glm::vec3(70.0f, 0.0f, -35.0f);
    } 
}

// Handles all of the keys you can input.
// When 1 is pressed, make the camera inactive and lock at stationary locations
// When 2 is pressed, the camera becomes active, going to handle2
// When 3 is pressed, the camera starts to rotate as it enters Chopper Mode.
// When 4 is pressed, the camera is from the perspective of the flying plane.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_RELEASE) {
        switch(key) {
            case GLFW_KEY_1:
                active2 = false;
                active3 = false;
                active4 = false;
                look = stationaryLook;
                target = stationaryTarget;
                break;
            case GLFW_KEY_2:
                active2 = true;
                active3 = false;
                active4 = false;
                break;
            case GLFW_KEY_3:
                active3 = true;
                active2 = false;
                active4 = false;
                break;
            case GLFW_KEY_4:
                active4 = true;
                active2 = false;
                active3 = false;
                break;
            case GLFW_KEY_S:
                raycast = !raycast;
                break;
            default:
                break;
        }
    }
    if (active2) {
        handleControlled(key, mods);
        return;
    }
}

void View::init(map<string,util::PolygonMesh<VertexAttrib>>& meshes, sgraph::IScenegraph *scenegraph) {
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    window = glfwCreateWindow(800, 800, "Hogwarts", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, 
    [](GLFWwindow* window, int width,int height) {
        static_cast<View*>(glfwGetWindowUserPointer(window))->reshape(width,height);
    });
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
    program.createProgram(string("shaders/phong-multiple.vert"), string("shaders/phong-multiple.frag"));
    program.enable();
    shaderLocations = program.getAllShaderVariables();
    vector<string> keys;
    for (const auto& pair : scenegraph->getTextureMap()) {
        keys.push_back(pair.first);
    }
    glEnable(GL_TEXTURE_2D);
    for (string key: keys) {
        util::TextureImage* texture = scenegraph->getTextureMap()[key];
        unsigned int textureId;
        glGenTextures(1,&textureId);
        glBindTexture(GL_TEXTURE_2D,textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->getWidth(), texture->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,texture->getImage());
        glGenerateMipmap(GL_TEXTURE_2D);
        textureIds[key] = textureId;
    }
    map<string, string> shaderVarsToVertexAttribs;
    shaderVarsToVertexAttribs["vPosition"] = "position";
    for (typename map<string,util::PolygonMesh<VertexAttrib> >::iterator it=meshes.begin();
           it!=meshes.end();
           it++) {
        util::ObjectInstance * obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(shaderLocations,shaderVarsToVertexAttribs,it->second);
        objects[it->first] = obj;
    }
	int window_width, window_height;
    glfwGetFramebufferSize(window,&window_width,&window_height);
	projection = glm::perspective(glm::radians(60.0f),(float)window_width/window_height,0.1f,10000.0f);
    glViewport(0, 0, window_width,window_height);
    renderer = new sgraph::GLScenegraphRenderer(modelview,objects,shaderLocations);    
    raytracer = new ray::Raytracer(200, 200, modelview, meshes, scenegraph);
}

//Camera Mode 4 is handled here. Using the inverse of the renderer's animation to follow the plane.
void View::display(sgraph::IScenegraph *scenegraph) {
    program.enable();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    tick = tick + 1;
    renderer->setTick(tick);
    modelview.push(glm::mat4(1.0));
    handleChopper();
    if (active4) {
        glm::mat4 location = renderer->getAnimation();
        modelview.top() = modelview.top()
            * glm::lookAt(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f,0.0f,1.0f))
            * inverse(location);
    } else {
        modelview.top() = modelview.top() * glm::lookAt(look, target,glm::vec3(0.0f,1.0f,0.0f));
    }
    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    sgraph::GLLightRenderer lights(modelview, shaderLocations);
    lights.setTick(tick);
    scenegraph->getRoot()->accept(&lights);
    if (raycast) {
        std::vector<util::Light> rayLightsVec;
        lights.getAllLights(rayLightsVec);
        std::vector<util::Light*> rayLights;
        for (util::Light& light : rayLightsVec) {
            rayLights.push_back(&light);
        }
        raytracer->render(rayLights);
        raytracer->saveImage("raytracer.ppm");
        raycast = false;
        return;
    }
    lights.setLights();
    renderer->setTextureMap(textureIds);
    scenegraph->getRoot()->accept(renderer);
    modelview.pop();
    sgraph::TextScenegraphRenderer text;
    scenegraph->getRoot()->accept(&text);
    glFlush();
    program.disable();
    glfwSwapBuffers(window);
    glfwPollEvents();
    if (first) {
        cout << text.getOutput() << endl;
        first = false;
    }
}

bool View::shouldWindowClose() {
    return glfwWindowShouldClose(window);
}

void View::reshape(int width, int height) {
    glViewport(0, 0, width, height);
    projection = glm::perspective(
        glm::radians(60.0f),
        (float)width /height,
        0.1f,
        10000.0f
    );
}

void View::closeWindow() {
    for (map<string,util::ObjectInstance *>::iterator it=objects.begin();
           it!=objects.end();
           it++) {
          it->second->cleanup();
          delete it->second;
    } 
    glfwDestroyWindow(window);
    glfwTerminate();
}

void View::dispose() {
    closeWindow();
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}





