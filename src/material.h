#ifndef MATERIAL_H
#define MATERIAL_H

#include <vector>

#include <glm/glm.hpp>

#include "texture.h"

class Material {
 public:
  /**
   * default material with values from
   * https://registry.khronos.org/OpenGL-Refpages/es1.1/xhtml/glMaterial.xml
   */
  Material();
  ~Material();
  Material(const Material& other) = default;
  Material& operator=(const Material& other) = default;
  Material(Material&& other) = default;
  Material& operator=(Material&& other) = default;

  void AddTexture(const Texture& to_add);

  /**
   * @brief binds all the textures of the material
   *
   */
  void BindTextures() const;

  void ambient_reflectivity(const glm::vec3& in);
  const glm::vec3& ambient_reflectivity() const;

  void diffuse_reflectivity(const glm::vec3& in);
  const glm::vec3& diffuse_reflectivity() const;

  void specular_reflectivity(const glm::vec3& in);
  const glm::vec3& specular_reflectivity() const;

  void shininess(const float in);
  const float shininess() const;

 private:
  std::vector<Texture> textures_;
  glm::vec3 ambient_reflectivity_;
  glm::vec3 diffuse_reflectivity_;
  glm::vec3 specular_reflectivity_;
  float shininess_;  // specular glossiness exponent
};

#endif  // MATERIAL_H
