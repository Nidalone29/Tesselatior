#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <filesystem>

#include <GL/glew.h>

#include "common.h"

class Texture {
 public:
  /**
   * @brief Construct a new Texture object from a given path
   *
   * @param path
   */
  Texture(const std::filesystem::path& path, const TEXTURE_TYPE type);
  ~Texture();
  Texture(const Texture& other) = default;
  Texture& operator=(const Texture& other) = default;
  Texture(Texture&& other) = default;
  Texture& operator=(Texture&& other) = default;

  GLuint getID() const;

 private:
  GLuint _id;
  // Texture type article
  // https://help.poliigon.com/en/articles/1712652-what-are-the-different-texture-maps-for
  // https://assimp.sourceforge.net/lib_html/material_8h.html#a7dd415ff703a2cc53d1c22ddbbd7dde0
  TEXTURE_TYPE _type;
  unsigned char _data;
};

#endif  // TEXTURE_H
