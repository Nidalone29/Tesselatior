#include "boot.h"
#include "common.h"

Boot::Boot() : _initialized(false) {}

void Boot::init() {
  _model.load_mesh("models/boot/boot.obj");
  _initialized = true;
}

void Boot::render() {
  if (!_initialized) {
    init();
  }

  _model.render();
}
