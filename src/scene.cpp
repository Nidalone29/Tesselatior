#include "scene.h"
#include <vector>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Scene::Scene()
    : _ambient_light(AmbientLight(glm::vec3(1, 1, 1), glm::vec3(0.2))),
      _directional_light(DirectionalLight(
          glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), glm::vec3(0, 0, -1))) {
  std::cout << "scene created" << std::endl;
}

void Scene::addObject(const Object& to_add) {
  _objects.push_back(to_add);
}

// TODO improve by making this return a const reference
std::vector<Object>& Scene::getAllObjects() {
  return _objects;
}

void Scene::setAmbientLight(const AmbientLight& to_add) {
  _ambient_light = to_add;
}

const AmbientLight& Scene::getAmbientLight() const {
  return _ambient_light;
}

void Scene::setDirectionalLight(const DirectionalLight& to_add) {
  _directional_light = to_add;
}

const DirectionalLight& Scene::getDirectionalLight() const {
  return _directional_light;
}