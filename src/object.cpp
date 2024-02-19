#include "object.h"

#include <iostream>

#include "logger.h"

Object::Object(const Model& model) : model_(model) {
  LOG_TRACE("Object(const Model&)");
}

Object::~Object() {
  LOG_TRACE("~Object()");
}

const Model& Object::model() const {
  return model_;
}

void Object::model(const Model& model) {
  model_ = model;
}
