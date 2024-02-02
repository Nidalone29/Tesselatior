#include "texture.h"

#include <iostream>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "logger.h"
#include "common.h"

Texture::Texture(const std::filesystem::path& path, const TEXTURE_TYPE type)
    : _id(-1), _type(type) {
  LOG_TRACE("Texture(const std::filesystem::path&, const std::string&)");

  LOG_INFO("Loading texture from \"{}\" ", path.string());
  int width, height, channels;
  unsigned char* image = nullptr;

  stbi_set_flip_vertically_on_load(true);

  // Usa la libreria lodepng per caricare l'immagine png
  image = stbi_load(path.string().c_str(), &width, &height, &channels, 4);

  stbi_set_flip_vertically_on_load(false);

  // std::cout << width << " " << height << " " << channels < std::endl;

  // TODO refactor
  if (image == nullptr) {
    LOG_WARN("Failed to load texture \"{}\"", path.string());

    // TODO this "white.png" should be the path argument
    // path = white.png
    image = stbi_load("white.png", &width, &height, &channels, 4);
    if (image == nullptr) {
      LOG_ERROR("Failed to load default texture");
      std::exit(2);
    }
  }

  _data = *image;

  // Crea un oggetto Texture in OpenGL
  glGenTextures(1, &_id);

  // Collega la texture al target specifico (tipo)
  glBindTexture(GL_TEXTURE_2D, _id);

  // Passa le informazioni dell'immagine sulla GPU:
  // Target
  // Numero di livelli del mipmap (0 in questo caso)
  // Formato della texture
  // Larghezza
  // Altezza
  // 0
  // Formato dei pixel dell'immagine di input
  // Tipo di dati dei pixel dell'immagine di input
  // Puntatore ai dati
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, image);

  // Imposta il filtro da usare per la texture minification
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Imposta il filtro da usare per la texture magnification
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Unbinda la texture
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image);
  LOG_INFO("Texture has been created from \"{}\"", path.string());
}

Texture::~Texture() {
  LOG_TRACE("~Texture()");
}

GLuint Texture::getID() const {
  return _id;
}
