#include "skull.h"
#include "common.h"

Skull::Skull() : _initialized(false) {}

void Skull::init(void) {
  _model.load_mesh("models/skull.obj");
  _initialized = true;
}

void Skull::render(void) {
  if (!_initialized) init();

  _model.render();
}
