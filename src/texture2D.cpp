#include "texture2D.h"

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  // Libreria di suporto per leggere immagini

Texture2D::Texture2D() : _texture(-1), _valid(false) {}

bool Texture2D::load(const std::string& FileName) {
  int width, height, channels;
  unsigned char* image = nullptr;

  stbi_set_flip_vertically_on_load(true);

  // Usa la libreria lodepng per caricare l'immagine png
  image = stbi_load(FileName.c_str(), &width, &height, &channels, 4);

  stbi_set_flip_vertically_on_load(false);

  // std::cout << width << " " << height << " " << channels<<std::endl;

  if (image == nullptr) {
    std::cerr << " Failed to load texture " << FileName << std::endl;
    exit(0);
  }

  // Crea un oggetto Texture in OpenGL
  glGenTextures(1, &_texture);

  // Collega la texture al target specifico (tipo)
  glBindTexture(GL_TEXTURE_2D, _texture);

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

  _filename = FileName;
  _valid = true;

  stbi_image_free(image);

  return true;
}

void Texture2D::bind(int TextureUnit) const {
  GLenum unit = GL_TEXTURE0 + TextureUnit;

  // Attiviamo la TextureUnit da usare per il sampling
  glActiveTexture(unit);

  // Bindiamo la texture
  glBindTexture(GL_TEXTURE_2D, _texture);
}

bool Texture2D::is_valid(void) const { return _valid; }