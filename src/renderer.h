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
  void Render(const Scene& scene, const Camera& camera) const;

  void ToggleWireframe();
  const FrameBuffer& target();
  void ResizeTarget(const int width, const int height);
  void ResizeTarget(const float width, const float height);
  void SetClearColor(const float r, const float g, const float b);

  int* tess_level();
  float* displacement_height();
  int max_tessel_level() const;

  float* phong_alpha();

 private:
  // for wireframe
  GLint gl_mode_;

  FrameBuffer render_target_;

  int tess_level_;
  int max_tessel_level_;
  float displacement_height_;

  float alpha_ = 0.5;
};

#endif  // RENDERER_H
