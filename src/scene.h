#ifndef SCENE_H
#define SCENE_H

#include "object.h"
#include "light.h"

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

  void setAmbientLight(const AmbientLight& to_add);
  const AmbientLight& getAmbientLight() const;
  void setDirectionalLight(const DirectionalLight& to_add);
  const DirectionalLight& getDirectionalLight() const;

 private:
  std::vector<Object> _objects;

  // there is only one ambient light
  AmbientLight _ambient_light;
  // there could probably be multiple directional lights?
  DirectionalLight _directional_light;
};

#endif  // SCENE_H
