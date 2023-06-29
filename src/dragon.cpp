#include "dragon.h"
#include "common.h"

Dragon::Dragon() : _initialized(false) {}

void Dragon::init() {
  _model.load_mesh("models/dragon.obj");
  _initialized = true;
}

void Dragon::render() {
  if (!_initialized) {
    init();
  }

  _model.render();
}
