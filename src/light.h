#ifndef LIGHT_H
#define LIGHT_H

#include "glm/glm.hpp"

class Light {
 public:
  const glm::vec3& getColor() const;
  const glm::vec3& getIntensity() const;

  void setColor(const glm::vec3& color);
  void setIntensity(const glm::vec3& intensity);

 protected:
  /**
   * @brief Construct a new Light object with a white color and 20% intensity
   *
   */
  Light();
  Light(const glm::vec3& color, const glm::vec3& intensity);

 private:
  glm::vec3 _color;
  glm::vec3 _intensity;  // the intensity of all 3 color channels
};

class AmbientLight : public Light {
 public:
  AmbientLight();
  AmbientLight(const glm::vec3& color, const glm::vec3& intensity);

 private:
};

class DirectionalLight : public Light {
 public:
  DirectionalLight(const glm::vec3& direction);

  DirectionalLight(const glm::vec3& color, const glm::vec3& intensity,
                   const glm::vec3& direction);
  const glm::vec3& getDirection() const;

 private:
  glm::vec3 _direction_vector;
};

class PointLight : public Light {
 public:
 private:
  glm::vec3 _position;
  float _radius;
};

#endif  // LIGHT_H