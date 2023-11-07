#include "framebuffer.h"

#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>

FrameBuffer::FrameBuffer(const int width, const int height)
    : _width(width), _height(height) {
  create();
  check();
}

FrameBuffer::~FrameBuffer() {
  reset();
}

void FrameBuffer::bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glViewport(0, 0, _width, _height);
}

void FrameBuffer::unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resize(const int new_width, const int new_height) {
  _width = new_width;
  _height = new_height;
  reset();
  create();
  check();
}

GLuint FrameBuffer::getTexture() const {
  return _color_attachment;
}

glm::vec2 FrameBuffer::getSize() const {
  return {_width, _height};
}

bool FrameBuffer::check() const {
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

  // TODO error handle with exception
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "easy" << std::endl;
  } else {
    std::cout << "fuckup" << std::endl;
    std::exit(-1);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return true;
}

void FrameBuffer::reset() {
  glDeleteFramebuffers(1, &_fbo);
  glDeleteTextures(1, &_color_attachment);
  glDeleteTextures(1, &_depth_stencil_attachment);
}

void FrameBuffer::create() {
  glGenFramebuffers(1, &_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

  glGenTextures(1, &_color_attachment);
  glBindTexture(GL_TEXTURE_2D, _color_attachment);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // attach the texture to the framebuffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         _color_attachment, 0);

  glGenTextures(1, &_depth_stencil_attachment);
  glBindTexture(GL_TEXTURE_2D, _depth_stencil_attachment);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, _width, _height, 0,
               GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                         GL_TEXTURE_2D, _depth_stencil_attachment, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}