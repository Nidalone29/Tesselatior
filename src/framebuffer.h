#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <map>
#include <gl/glew.h>

class Attachment {
 public:
  const GLuint& getAttachmentID() const;
  virtual const GLuint& bind();

 protected:
  Attachment();
  GLuint _attachment_id;

 private:
};

class ColorAttachment : public Attachment {
 public:
  const GLuint& bind();

 private:
  // poossible texture values
};

class DepthStencilAttachment : public Attachment {
 public:
  const GLuint& bind();

 private:
  // poossible texture values
};

/**
 * @brief the opengl framebuffer that we will submit to a render target (the
 * screen, an image for imgui viewport...)
 *
 * it can contain multiple attachments (Colorbuffer, Depthbuffer....)
 *
 */
class FrameBuffer {
 public:
  FrameBuffer(const int width, const int height);
  ~FrameBuffer();

  /**
   * @brief
   *
   * @param type GL_COLOR_ATTACHMENT0 | GL_DEPTH24_STENCIL8 (only supported for
   * now)
   * @param to_add
   */
  void addAttachment(const GLenum type, const Attachment& to_add);
  // void removeAttachment(const Attachment& to_add);

  /**
   * @brief bind the framebuffer and all it's attachments?
   *
   */
  void bind();
  void unbind();

  bool check();
  void resize(const int new_width, const int new_height);

 private:
  int _width, _height;
  std::map<GLenum, Attachments> _attachments;

  // Frame Buffer Object
  GLuint _fbo;
};

#endif  // FRAMEBUFFER_H