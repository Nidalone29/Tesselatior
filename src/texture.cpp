#include "texture.h"

#include <iostream>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <assimp/texture.h>

#include "logger.h"
#include "utilities.h"

Texture::Texture(const TEXTURE_TYPE type) : id_(-1), type_(type) {
  LOG_TRACE("Texture(const TEXTURE_TYPE)");

  LOG_INFO("Loading default texture");
  int width, height, channels;
  unsigned char* image = nullptr;

  stbi_set_flip_vertically_on_load(true);

  // 4 means desired channels, in this case we want 4 because RGBA
  image = stbi_load("white.png", &width, &height, &channels, 4);
  if (image == nullptr) {
    LOG_ERROR("Failed to load default texture");
    throw FileNotFoundException();
  }

  stbi_set_flip_vertically_on_load(false);

  data_ = *image;

  glGenTextures(1, &id_);

  glBindTexture(GL_TEXTURE_2D, id_);

  // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image);
  LOG_INFO("Default texture has been created");
}

Texture::Texture(const std::filesystem::path& path, const TEXTURE_TYPE type)
    : id_(-1), type_(type) {
  LOG_TRACE("Texture(const std::filesystem::path&, const TEXTURE_TYPE)");

  LOG_INFO("Loading texture from \"{}\" ", path.string());
  int width, height, channels;
  unsigned char* image = nullptr;

  stbi_set_flip_vertically_on_load(true);

  image = stbi_load(path.string().c_str(), &width, &height, &channels, 4);

  stbi_set_flip_vertically_on_load(false);

  // TODO refactor
  if (image == nullptr) {
    LOG_WARN("Failed to load texture \"{}\"", path.string());

    // TODO this "white.png" should be the path argument
    // path = white.png
    image = stbi_load("white.png", &width, &height, &channels, 4);
    if (image == nullptr) {
      LOG_ERROR("Failed to load default texture");
      throw FileNotFoundException();
    }
  }

  data_ = *image;

  glGenTextures(1, &id_);

  glBindTexture(GL_TEXTURE_2D, id_);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image);
  LOG_INFO("Texture has been created from \"{}\"", path.string());
}

Texture::Texture(const aiTexture* embedded, const TEXTURE_TYPE type)
    : id_(-1), type_(type) {
  LOG_TRACE("Texture(const aiTexture*, const TEXTURE_TYPE)");
  LOG_INFO("Loading embedded texture from \"{}\"", embedded->mFilename.C_Str());

  int width, height, channels;
  unsigned char* image = nullptr;

  stbi_set_flip_vertically_on_load(true);

  image =
      stbi_load_from_memory((unsigned char*)embedded->pcData, embedded->mWidth,
                            &width, &height, &channels, 4);

  stbi_set_flip_vertically_on_load(false);

  // TODO refactor
  if (image == nullptr) {
    LOG_ERROR("Failed to load texture \"{}\"", embedded->mFilename.C_Str());
    image = stbi_load("white.png", &width, &height, &channels, 4);
    if (image == nullptr) {
      LOG_ERROR("Failed to load default texture");
      throw FileNotFoundException();
    }
  }

  data_ = *image;

  glGenTextures(1, &id_);

  glBindTexture(GL_TEXTURE_2D, id_);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image);
  LOG_INFO("Texture has been created from \"{}\"", embedded->mFilename.C_Str());
}

Texture::~Texture() {
  LOG_TRACE("~Texture()");
}

GLuint Texture::id() const {
  return id_;
}
