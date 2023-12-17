#include "object.h"

#include <iostream>

Object::Object(const Model& model) : _model(model) {
  std::cout << "object created" << std::endl;
}

Object::~Object() {}

const Model& Object::getModel() const {
  return _model;
}

Model& Object::getModModel() {
  return _model;
}

void Object::setModel(const Model& model) {
  _model = model;
}
