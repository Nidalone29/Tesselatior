#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include "texture.h"
#include <vector>

class Material {
 public:
  /** default material with values from
   * https://registry.khronos.org/OpenGL-Refpages/es1.1/xhtml/glMaterial.xml
   */
  Material();
  ~Material() = default;
  Material(const Material& other) = default;
  Material& operator=(const Material& other) = default;
  Material(Material&& other) = default;
  Material& operator=(Material&& other) = default;

  void addTexture(const Texture& toadd);

  /**
   * @brief binds all the textures of the material
   *
   */
  void bind() const;

  void setAmbientReflectivity(const glm::vec3& in);
  const glm::vec3& getAmbientReflectivity() const;

  void setDiffuseReflectivity(const glm::vec3& in);
  const glm::vec3& getDiffuseReflectivity() const;

  void setSpecularReflectivity(const glm::vec3& in);
  const glm::vec3& getSpecularReflectivity() const;

  void setGlossinessExponent(const float& in);
  const float& getGlossinessExponent() const;

 private:
  std::vector<Texture> _textures;
  glm::vec3 _ambient_reflectivity;
  glm::vec3 _diffuse_reflectivity;
  glm::vec3 _specular_reflectivity;
  float _specular_glossiness_exponent;
};

#endif  // MATERIAL_H