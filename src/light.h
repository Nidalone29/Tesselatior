#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light {
 public:
  const glm::vec3& color() const;
  float* color();
  void color(const glm::vec3& color);

  const glm::vec3& intensity() const;
  float* intensity();
  void intensity(const glm::vec3& intensity);

 protected:
  /**
   * @brief Construct a new Light object with a white color and 20% intensity
   *
   */
  Light();
  Light(const glm::vec3& color, const glm::vec3& intensity);

 private:
  glm::vec3 color_;
  glm::vec3 intensity_;  // the intensity of all 3 color channels
};

class AmbientLight : public Light {
 public:
  AmbientLight();
  AmbientLight(const glm::vec3& color, const glm::vec3& intensity);
};

class DirectionalLight : public Light {
 public:
  DirectionalLight();

  explicit DirectionalLight(const glm::vec3& direction);

  DirectionalLight(const glm::vec3& color, const glm::vec3& intensity,
                   const glm::vec3& direction);

  void direction(const glm::vec3& in);
  const glm::vec3& direction() const;
  float* direction();

 private:
  glm::vec3 direction_vector_;
};

#endif  // LIGHT_H
