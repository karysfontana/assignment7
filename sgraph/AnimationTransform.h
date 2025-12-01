#ifndef _ANIMATIONNODE_H_
#define _ANIMATIONNODE_H_

#include "TransformNode.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

namespace sgraph
{
  class AnimationTransform : public TransformNode {
  public:
    AnimationTransform(const string& name, sgraph::IScenegraph *graph)
      : TransformNode(name, graph) {
      this->transform = glm::mat4(1.0f);
    }

    virtual ~AnimationTransform() {}

    void accept(SGNodeVisitor *visitor) override {
      visitor->visitAnimationTransform(this);
    }

    //This calculate where the animated object currently is using ticks.
    virtual void setAnimation(int tick)=0;
  };
}
#endif
