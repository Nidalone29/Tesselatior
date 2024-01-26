#include "scene.h"

#include <vector>
#include <iostream>

#include <glm/gtx/string_cast.hpp>

#include "logger.h"

Scene::Scene()
    : _name("unnamed scene"),
      _ambient_light(AmbientLight(glm::vec3(1, 1, 1), glm::vec3(0.2F))),
      _directional_light(DirectionalLight(
          glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), glm::vec3(0, 0, -1))) {
  LOG_TRACE("Scene()");
}

Scene::~Scene() {
  LOG_TRACE("~Scene()");
  LOG_TRACE("Destroying scene \"{}\"", _name);
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

void Scene::setName(const std::string& to_add) {
  _name = to_add;
}
const std::string& Scene::getName() const {
  return _name;
}