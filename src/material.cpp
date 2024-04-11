#include "material.h"

#include <iostream>

#include <GL/glew.h>

#include "logger.h"
#include "utilities.h"

Material::Material()
    : ambient_reflectivity_(glm::vec3(1.0F, 1.0F, 1.0F)),
      diffuse_reflectivity_(glm::vec3(1.0F, 1.0F, 1.0F)),
      specular_reflectivity_(glm::vec3(0.0F, 0.0F, 0.0F)),
      shininess_(30.0F) {
  LOG_TRACE("Material()");
}

Material::~Material() {
  LOG_TRACE("~Material()");
}

void Material::AddTexture(const Texture& to_add) {
  textures_.push_back(to_add);
}

void Material::BindTextures() const {
  glBindTexture(GL_TEXTURE_2D, 0);
  for (const Texture& t : textures_) {
    GLenum unit = GL_TEXTURE0 + to_underlying(t.type());

    // Attiviamo la TextureUnit da usare per il sampling
    glActiveTexture(unit);
    // Bindiamo la texture
    glBindTexture(GL_TEXTURE_2D, t.id());
  }
}

void Material::ambient_reflectivity(const glm::vec3& in) {
  ambient_reflectivity_ = in;
}

const glm::vec3& Material::ambient_reflectivity() const {
  return ambient_reflectivity_;
}

void Material::diffuse_reflectivity(const glm::vec3& in) {
  diffuse_reflectivity_ = in;
}

const glm::vec3& Material::diffuse_reflectivity() const {
  return diffuse_reflectivity_;
}

void Material::specular_reflectivity(const glm::vec3& in) {
  specular_reflectivity_ = in;
}

const glm::vec3& Material::specular_reflectivity() const {
  return specular_reflectivity_;
}

void Material::shininess(const float in) {
  shininess_ = in;
}

const float Material::shininess() const {
  return shininess_;
}
