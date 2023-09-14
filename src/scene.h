#ifndef SCENE_H
#define SCENE_H

#include "object.h"

// a scene graph is not needed for this application

class Scene {
 public:
  Scene();
  // temp
  Scene(const Scene& other) = delete;
  Scene& operator=(const Scene& other) = delete;
  Scene(Scene&& other) = delete;
  Scene& operator=(Scene&& other) = delete;

  void addObject(const Object& to_add);
  std::vector<Object>& getAllObjects();

  /**
   * @brief Loads the scene to the OpenGL contex
   *
   */
  // void load();

 private:
  std::vector<Object> _objects;

  // true iff the scene got loaded
  bool _loaded;
};

#endif  // SCENE_H
