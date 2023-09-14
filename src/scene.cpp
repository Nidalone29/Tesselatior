#include "scene.h"
#include <vector>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

Scene::Scene() {
  std::cout << "scene created" << std::endl;
}

void Scene::addObject(const Object& to_add) {
  _objects.push_back(to_add);
}

// TODO improve by making this return a const reference
std::vector<Object>& Scene::getAllObjects() {
  return _objects;
}
