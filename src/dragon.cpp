#include "dragon.h"
#include "common.h"

Dragon::Dragon() : _initialized(false) {}

void Dragon::init(void) {
  _model.load_mesh("models/dragon.obj");
  _initialized = true;
}

void Dragon::render(void) {
  if (!_initialized) init();

  _model.render();
}
