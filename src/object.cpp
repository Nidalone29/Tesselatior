#include "object.h"

#include <iostream>

#include "logger.h"

Object::Object(const Model& model) : _model(model) {
  LOG_TRACE("Object(const Model&)");
}

Object::~Object() {
  LOG_TRACE("~Object()");
}

const Model& Object::getModel() const {
  return _model;
}

void Object::setModel(const Model& model) {
  _model = model;
}
