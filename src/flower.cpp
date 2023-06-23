#include "flower.h"
#include "common.h"

Flower::Flower() : _initialized(false) {}

void Flower::init(void) {
  _model.load_mesh("models/flower/flower.obj", aiProcess_Triangulate);
  _initialized = true;
}

void Flower::render(void) {
  if (!_initialized) init();

  _model.render();
}
