#include "teapot.h"
#include "common.h"

Teapot::Teapot() : _initialized(false) {}

void Teapot::init() {
  _model.load_mesh("models/teapot.obj");
  _initialized = true;
}

void Teapot::render() {
  if (!_initialized) {
    init();
  }

  _model.render();
}
