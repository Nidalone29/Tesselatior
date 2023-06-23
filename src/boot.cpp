#include "boot.h"
#include "common.h"

Boot::Boot() : _initialized(false) {}

void Boot::init(void) {
  _model.load_mesh("models/boot/boot.obj");
  _initialized = true;
}

void Boot::render(void) {
  if (!_initialized) init();

  _model.render();
}
