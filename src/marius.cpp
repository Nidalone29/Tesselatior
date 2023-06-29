#include "marius.h"
#include "common.h"

Marius::Marius() : _initialized(false) {}

void Marius::init() {
  _head.load_mesh("models/marius/head.obj", aiProcess_FlipUVs);
  _eyes.load_mesh("models/marius/eyes.obj", aiProcess_FlipUVs);
  _eyebrows.load_mesh("models/marius/eyebrows.obj", aiProcess_FlipUVs);
  _hairplate.load_mesh("models/marius/hair_plate.obj", aiProcess_FlipUVs);
  _eyelashesLower.load_mesh("models/marius/eyelashesLower.obj",
                            aiProcess_FlipUVs);
  _eyelashesUpper.load_mesh("models/marius/eyelashesUpper.obj",
                            aiProcess_FlipUVs);
  _initialized = true;
}

void Marius::render() {
  if (!_initialized) {
    init();
  }

  _head.render();

  _eyes.render();

  glEnable(GL_BLEND);
  glEnable(GL_ALPHA_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  _eyebrows.render();
  _hairplate.render();
  _eyelashesUpper.render();
  _eyelashesLower.render();
  glDisable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
}
