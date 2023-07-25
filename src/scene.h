#ifndef SCENE_H
#define SCENE_H

#include "model.h"
#include "mesh.h"

// a scene graph is not needed for this application

class Scene {
 public:
  Scene();

  void addObject(const Object& to_add);
  const std::vector<Object>& getAllRenderableObjects() const;

 private:
  std::vector<Object> _objects;
};

#endif  // SCENE_H