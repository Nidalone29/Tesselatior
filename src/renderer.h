#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>

#include "scene.h"
#include "shader.h"
#include "framebuffer.h"
#include "camera.h"

class Renderer {
 public:
  Renderer();
  ~Renderer();
  void Render(const Scene& scene, const Camera& camera,
              const Shader& shader) const;

  void ToggleWireframe();
  const FrameBuffer& target();
  void ResizeTarget(const int width, const int height);
  void ResizeTarget(const float width, const float height);
  // void clear_color(const float r, const float g, const float b, const float
  // a);

 private:
  // for wireframe
  GLint gl_mode_;

  FrameBuffer render_target_;
};

#endif  // RENDERER_H
