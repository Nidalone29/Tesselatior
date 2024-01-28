#ifndef SCENE_H
#define SCENE_H

#include "object.h"
#include "light.h"

// a scene graph is not needed for this application

class Scene {
 public:
  explicit Scene(const std::string& name = "unnamed scene");
  ~Scene();
  // temp
  Scene(const Scene& other) = default;
  Scene& operator=(const Scene& other) = default;
  Scene(Scene&& other) = default;
  Scene& operator=(Scene&& other) = default;

  void addObject(const Object& to_add);
  const std::vector<Object>& getAllObjects() const;

  void setAmbientLight(const AmbientLight& to_add);
  const AmbientLight& getAmbientLight() const;
  void setDirectionalLight(const DirectionalLight& to_add);
  const DirectionalLight& getDirectionalLight() const;

  void setName(const std::string& to_add);
  const std::string& getName() const;

 private:
  std::string _name;
  std::vector<Object> _objects;

  // there is only one ambient light
  AmbientLight _ambient_light;
  // there could probably be multiple directional lights?
  DirectionalLight _directional_light;
};

#endif  // SCENE_H
