#include "framebuffer.h"

#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "logger.h"
#include "utilities.h"

FrameBuffer::FrameBuffer(const int width, const int height)
    : width_(width), height_(height) {
  LOG_TRACE("FrameBuffer(const int, const int)");
  Create();
  Check();
}

FrameBuffer::~FrameBuffer() {
  LOG_TRACE("~FrameBuffer()");
  Reset();
}

void FrameBuffer::Bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glViewport(0, 0, width_, height_);
}

void FrameBuffer::Unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Resize(const int new_width, const int new_height) {
  width_ = new_width;
  height_ = new_height;
  Reset();
  Create();
  Check();
}

GLuint FrameBuffer::color_attachment_id() const {
  return color_attachment_;
}

glm::vec2 FrameBuffer::size_vector() const {
  return {width_, height_};
}

void FrameBuffer::Check() const {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    LOG_ERROR("Framebuffer status error");
    throw FramebufferException();
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Reset() {
  glDeleteFramebuffers(1, &fbo_);
  glDeleteTextures(1, &color_attachment_);
  glDeleteTextures(1, &depth_stencil_attachment_);
}

void FrameBuffer::Create() {
  glGenFramebuffers(1, &fbo_);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

  glGenTextures(1, &color_attachment_);
  glBindTexture(GL_TEXTURE_2D, color_attachment_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // attach the texture to the framebuffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         color_attachment_, 0);

  glGenTextures(1, &depth_stencil_attachment_);
  glBindTexture(GL_TEXTURE_2D, depth_stencil_attachment_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width_, height_, 0,
               GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                         GL_TEXTURE_2D, depth_stencil_attachment_, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
