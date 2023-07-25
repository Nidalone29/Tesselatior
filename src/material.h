#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>

class Material {
 public:
  /** default material with values from
   * https://registry.khronos.org/OpenGL-Refpages/es1.1/xhtml/glMaterial.xml
   * */
  Material();
  ~Material() = default;
  Material(const Material& other) = default;
  Material& operator=(const Material& other) = default;
  Material(Material&& other) = default;
  Material& operator=(Material&& other) = default;

  const glm::vec3& getAmbientColor() const;
  const glm::vec3& getDiffuseColor() const;
  const glm::vec3& getSpecularColor() const;
  const float& getShininess() const;

 private:
  glm::vec3 _ambient_color;
  glm::vec3 _diffuse_color;
  glm::vec3 _specular_color;
  //  glm::vec3 emissive; // TODO for a kind of light maybe?
  float _shininess_exponent;  // 0 not shiny 128 very shiny
};

#endif  // MATERIAL_H