#include "scene.h"

Scene::Scene() {}

void Scene::addObject(const Object& to_add) {
  _objects.push_back(to_add);
}

const std::vector<Object>& Scene::getAllRenderableObjects() const {
  return _objects;
}