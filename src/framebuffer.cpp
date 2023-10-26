#include "framebuffer.h"

FrameBuffer::FrameBuffer(const int width, const int height)
    : _width(width), _height(height) {
  //
}

FrameBuffer::~FrameBuffer() {}

void FrameBuffer::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void FrameBuffer::unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::addAttachment(const GLenum type, const Attachment& to_add) {
  _attachments.insert({type, to_add});
}

const GLuint& Attachment::getAttachmentID() const {
  return _attachment_id;
}

const GLuint& ColorAttachment::bind() {
  glBindTexture(GL_TEXTURE_2D, _attachment_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, viewport_width, viewport_height, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

const GLuint& DepthStencilAttachment::bind() {
  glBindTexture(GL_TEXTURE_2D, _attachment_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, viewport_width,
               viewport_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,
               nullptr);
}
