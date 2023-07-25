#include "renderer.h"

#include "mesh.h"

Renderer::Renderer() : _gl_mode(GL_FILL) {}

void Renderer::toggleWireframe() {
  _gl_mode = (_gl_mode == GL_FILL) ? GL_LINE : GL_FILL;

  glPolygonMode(GL_FRONT_AND_BACK, _gl_mode);
}

void Renderer::render(Scene scene) {
  std::vector<Object> x = scene.getAllRenderableObjects();
  for (Mesh m : x) {
    m.render();  // uhm...
  }
}
