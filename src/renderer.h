#ifndef RENDERER_H
#define RENDERER_H

#include "scene.h"
#include "shader.h"
#include "framebuffer.h"
#include "camera.h"
#include <GL/glew.h>

class Renderer {
 public:
  Renderer();
  ~Renderer() = default;
  void render(Scene& scene, const Camera& camera, const Shader& shader) const;

  void toggleWireframe();
  const FrameBuffer& target();
  void resizeTarget(const int width, const int height);
  void setClearColor(const float r, const float g, const float b,
                     const float a);

 private:
  // for wireframe
  GLint _gl_mode;

  FrameBuffer _render_target;
};

#endif  // RENDERER_H