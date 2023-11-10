#ifndef RENDERER_H
#define RENDERER_H

#include "scene.h"
#include "shader.h"
#include <GL/glew.h>

class Renderer {
 public:
  Renderer();
  ~Renderer() = default;
  void render(Scene& scene, const Shader& shader) const;

  void toggleWireframe();

 private:
  // for wireframe
  GLint _gl_mode;

  FrameBuffer _render_target;
};

#endif  // RENDERER_H