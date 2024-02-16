#include "material.h"

#include <iostream>

#include <GL/glew.h>

#include "logger.h"

Material::Material()
    : _ambient_reflectivity(glm::vec3(1.0F, 1.0F, 1.0F)),
      _diffuse_reflectivity(glm::vec3(1.0F, 1.0F, 1.0F)),
      _specular_reflectivity(glm::vec3(0.0F, 0.0F, 0.0F)),
      _specular_glossiness_exponent(30.0F) {
  LOG_TRACE("Material()");
}

Material::~Material() {
  LOG_TRACE("~Material()");
}

void Material::addTexture(const Texture& toadd) {
  _textures.push_back(toadd);
}

void Material::bind() const {
  glBindTexture(GL_TEXTURE_2D, 0);
  for (const Texture& t : _textures) {
    // (for now it's 0, meaning it's all color texture)
    // TODO a check for all different types of textures and deal with it
    GLenum unit = GL_TEXTURE0;

    // Attiviamo la TextureUnit da usare per il sampling
    glActiveTexture(unit);
    // Bindiamo la texture
    glBindTexture(GL_TEXTURE_2D, t.getID());
  }
}

void Material::setAmbientReflectivity(const glm::vec3& in) {
  _ambient_reflectivity = in;
}

const glm::vec3& Material::getAmbientReflectivity() const {
  return _ambient_reflectivity;
}

void Material::setDiffuseReflectivity(const glm::vec3& in) {
  _diffuse_reflectivity = in;
}

const glm::vec3& Material::getDiffuseReflectivity() const {
  return _diffuse_reflectivity;
}

void Material::setSpecularReflectivity(const glm::vec3& in) {
  _specular_reflectivity = in;
}

const glm::vec3& Material::getSpecularReflectivity() const {
  return _specular_reflectivity;
}

void Material::setGlossinessExponent(const float& in) {
  _specular_glossiness_exponent = in;
}

const float& Material::getGlossinessExponent() const {
  return _specular_glossiness_exponent;
}
