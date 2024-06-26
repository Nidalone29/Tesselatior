#include "scene.h"

#include <vector>
#include <iostream>

#include <glm/gtx/string_cast.hpp>

#include "logger.h"

Scene::Scene(const std::string& name)
    : name_(name),
      ambient_light_(
          AmbientLight(glm::vec3(1.0F, 1.0F, 1.0F), glm::vec3(0.2F))),
      directional_light_(DirectionalLight(glm::vec3(1.0F, 1.0F, 1.0F),
                                          glm::vec3(1.0F, 1.0F, 1.0F),
                                          glm::vec3(-1.0F, -1.0F, -1.0F))) {
  LOG_TRACE("Scene(const std::string&)");
}

Scene::~Scene() {
  LOG_TRACE("~Scene()");
  LOG_TRACE("Destroying scene \"{}\"", name_);
  for (const IRenderableObject* x : objects_) {
    delete x;
  }
}

void Scene::AddObject(IRenderableObject* to_add) {
  objects_.push_back(to_add);
}

const std::vector<IRenderableObject*> Scene::objects() const {
  return objects_;
}

const AmbientLight& Scene::ambient_light() const {
  return ambient_light_;
}

AmbientLight* Scene::ambient_light() {
  return &ambient_light_;
}

void Scene::ambient_light(const AmbientLight& to_add) {
  ambient_light_ = to_add;
}

const DirectionalLight& Scene::directional_light() const {
  return directional_light_;
}

DirectionalLight* Scene::directional_light() {
  return &directional_light_;
}

void Scene::directional_light(const DirectionalLight& to_add) {
  directional_light_ = to_add;
}

void Scene::name(const std::string& to_add) {
  name_ = to_add;
}

const std::string& Scene::name() const {
  return name_;
}

int Scene::NumberOfObjects() const {
  return objects_.size();
}
