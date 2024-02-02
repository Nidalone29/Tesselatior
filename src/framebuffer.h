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
  void bind() const;
  void unbind() const;

  /**
   * @brief Retrieved the texture where the framebuffer has rendered to
   *
   * @return const GLuint
   */
  GLuint getTexture() const;

  glm::vec2 getSize() const;
  void resize(const int new_width, const int new_height);

 private:
  void check() const;
  void reset();
  void create();

  int _width, _height;

  GLuint _color_attachment;
  GLuint _depth_stencil_attachment;

  // Frame Buffer Object
  GLuint _fbo;
};

#endif  // FRAMEBUFFER_H
