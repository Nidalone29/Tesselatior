#include "light.h"

#include "logger.h"

Light::Light() : _color(glm::vec3(1.0F)), _intensity(glm::vec3(0.2F)) {
  //
}

Light::Light(const glm::vec3& color, const glm::vec3& intensity)
    : _color(color), _intensity(intensity) {
  //
}

const glm::vec3& Light::getColor() const {
  return _color;
}

const glm::vec3& Light::getIntensity() const {
  return _intensity;
}

void Light::setColor(const glm::vec3& color) {
  _color = color;
}

void Light::setIntensity(const glm::vec3& intensity) {
  _intensity = intensity;
}

AmbientLight::AmbientLight() : Light() {
  LOG_TRACE("AmbientLight()");
}

AmbientLight::AmbientLight(const glm::vec3& color, const glm::vec3& intensity)
    : Light(color, intensity) {
  LOG_TRACE("AmbientLight(const glm::vec3&, const glm::vec3&)");
}

DirectionalLight::DirectionalLight()
    : Light(), _direction_vector(0.0F, 0.0F, 0.0F) {
  LOG_TRACE("DirectionalLight()");
}

DirectionalLight::DirectionalLight(const glm::vec3& direction)
    : Light(), _direction_vector(direction) {
  LOG_TRACE("DirectionalLight(const glm::vec3&)");
}

const glm::vec3& DirectionalLight::getDirection() const {
  return _direction_vector;
}

DirectionalLight::DirectionalLight(const glm::vec3& color,
                                   const glm::vec3& intensity,
                                   const glm::vec3& direction)
    : Light(color, intensity), _direction_vector(direction) {
  //
}
