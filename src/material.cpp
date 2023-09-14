#include "material.h"
#include <gl/glew.h>
#include <iostream>

Material::Material() {
  std::cout << "material created" << std::endl;
}

void Material::addTexture(const Texture& toadd) {
  _textures.push_back(toadd);
}

void Material::bind() const {
  for (Texture t : _textures) {
    // (for now it's 0, meaning it's all color texture)
    // TODO a check for all different types of textures and deal with it
    GLenum unit = GL_TEXTURE0;

    // Attiviamo la TextureUnit da usare per il sampling
    glActiveTexture(unit);

    // Bindiamo la texture
    glBindTexture(GL_TEXTURE_2D, t.getID());
  }
}
