#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

Texture::Texture(const std::string& path, const std::string& type)
    : _type(type), _id(-1) {
  int width, height, channels;
  unsigned char* image = nullptr;

  stbi_set_flip_vertically_on_load(true);

  // Usa la libreria lodepng per caricare l'immagine png
  image = stbi_load(path.c_str(), &width, &height, &channels, 4);

  stbi_set_flip_vertically_on_load(false);

  // std::cout << width << " " << height << " " << channels < std::endl;

  if (image == nullptr) {
    // TODO it should load the default texture "./models/default_texture.png"

    std::cerr << " Failed to load texture " << path << std::endl;
    exit(0);
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
}

GLuint Texture::getID() const {
  return _id;
}
