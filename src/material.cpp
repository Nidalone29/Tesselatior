#include "material.h"

Material::Material() {}

const glm::vec3& Material::getAmbientColor() const {
  return _ambient_color;
}

const glm::vec3& Material::getDiffuseColor() const {
  return _diffuse_color;
}

const glm::vec3& Material::getSpecularColor() const {
  return _specular_color;
}

const float& Material::getShininess() const {
  return _shininess_exponent;
}