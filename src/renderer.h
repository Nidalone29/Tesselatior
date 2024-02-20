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
  void SetClearColor(const float r, const float g, const float b);

  int* tess_level_inner0();
  int* tess_level_outer0();
  int* tess_level_outer1();
  int* tess_level_outer2();

 private:
  // for wireframe
  GLint gl_mode_;

  FrameBuffer render_target_;

  int tess_level_inner0_ = 0.0;

  int tess_level_outer0_ = 1.0;
  int tess_level_outer1_ = 1.0;
  int tess_level_outer2_ = 1.0;
};

#endif  // RENDERER_H
