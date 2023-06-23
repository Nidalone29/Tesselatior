#include "teapot.h"
#include "common.h"

Teapot::Teapot() : _initialized(false) {}

void Teapot::init(void) {
  _model.load_mesh("models/teapot.obj");
  _initialized = true;
}

void Teapot::render(void) {
  if (!_initialized) init();

  _model.render();
}
