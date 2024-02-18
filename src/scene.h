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

  void AddObject(const Object& to_add);
  const std::vector<Object>& objects() const;

  const AmbientLight& ambient_light() const;
  AmbientLight* ambient_light();
  void ambient_light(const AmbientLight& to_add);

  const DirectionalLight& directional_light() const;
  DirectionalLight* directional_light();
  void directional_light(const DirectionalLight& to_add);

  void name(const std::string& to_add);
  const std::string& name() const;

 private:
  std::string name_;
  std::vector<Object> objects_;

  // There is only one ambient light
  AmbientLight ambient_light_;
  // The sun?
  DirectionalLight directional_light_;
};

#endif  // SCENE_H
