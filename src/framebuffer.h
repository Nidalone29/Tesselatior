#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

/**
 * @brief the opengl framebuffer that we will submit to a render target (the
 * screen, an image for imgui viewport...)
 *
 * Class used to render a 3D scene to a _single_ Texture. It supports resizing
 * of the viewport
 *
 */
class FrameBuffer {
 public:
  FrameBuffer(const int width, const int height);
  ~FrameBuffer();

  /**
   * @brief bind the framebuffer
   *
   */
  void Bind() const;
  void Unbind() const;

  /**
   * @brief Retrieves the texture ID where the framebuffer has rendered to
   *
   * @return const GLuint
   */
  GLuint color_attachment_id() const;
  glm::vec2 size_vector() const;

  void Resize(const int new_width, const int new_height);

 private:
  void Check() const;
  void Reset();
  void Create();

  int width_, height_;

  GLuint color_attachment_;
  GLuint depth_stencil_attachment_;

  // Frame Buffer Object
  GLuint fbo_;
};

#endif  // FRAMEBUFFER_H
