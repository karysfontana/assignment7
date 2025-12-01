#ifndef _KEYFRAMEANIMATIONNODE_H_
#define _KEYFRAMEANIMATIONNODE_H_

#include "AnimationTransform.h"
#include <fstream>
#include <sstream>
#include <istream>

using namespace std;

namespace sgraph
{
  class KeyframeAnimationTransform : public AnimationTransform {
  protected:
    vector<glm::vec3> position;
    vector<glm::vec3> ups;
    string filename;

    ParentSGNode *copyNode() {
      return new KeyframeAnimationTransform(filename,name,scenegraph);
    }   

  public:
    KeyframeAnimationTransform(const string& filename, const string& name, sgraph::IScenegraph *graph)
      : AnimationTransform(name, graph) {
      this->filename = filename;
      load(filename);
    }

    virtual ~KeyframeAnimationTransform() {}
  
    //load is how you upload the keyframe file.
    // it places everything into the position vector or ups vector.
    void load(const string &filename) {
      ifstream infile(filename);
      int frames;
      infile >> frames;
      for (int i = 0; i < frames; i++) {
        float x, y, z, upx, upy, upz;
        infile >> x >> y >> z >> upx >> upy >> upz;
        position.push_back(glm::vec3(x, y, z));
        ups.push_back(glm::vec3(upx, upy, upz));
      }
      infile.close();
    }

    // Find description in AnimationTransform.h
    void setAnimation(int tick) override {
        int curr = tick % position.size();
        int next = (tick+1) % position.size();
        glm::vec3 pos = position[curr];
        glm::vec3 up = glm::normalize(ups[curr]);
        glm::vec3 forward = glm::normalize(position[next] - pos);
        glm::vec3 right = glm::normalize(glm::cross(up, forward));
        glm::mat4 rotation = glm::mat4(
          right.x, forward.x, up.x, 0.0f,
          right.y, forward.y, up.y, 0.0f,
          right.z, forward.z, up.z, 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f
        );
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), pos);
        glm::mat4 transform = translation * glm::inverse(rotation);
        setTransform(transform);
    }
  };
}
#endif
