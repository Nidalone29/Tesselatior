#include "light.h"

#include <glm/gtc/type_ptr.hpp>

#include "logger.h"

Light::Light() : color_(glm::vec3(1.0F)), intensity_(glm::vec3(0.2F)) {
  //
}

Light::Light(const glm::vec3& color, const glm::vec3& intensity)
    : color_(color), intensity_(intensity) {
  //
}

const glm::vec3& Light::color() const {
  return color_;
}

float* Light::color() {
  return glm::value_ptr(color_);
}

const glm::vec3& Light::intensity() const {
  return intensity_;
}

float* Light::intensity() {
  return glm::value_ptr(intensity_);
}

void Light::color(const glm::vec3& color) {
  color_ = color;
}

void Light::intensity(const glm::vec3& intensity) {
  intensity_ = intensity;
}

AmbientLight::AmbientLight() : Light() {
  LOG_TRACE("AmbientLight()");
}

AmbientLight::AmbientLight(const glm::vec3& color, const glm::vec3& intensity)
    : Light(color, intensity) {
  LOG_TRACE("AmbientLight(const glm::vec3&, const glm::vec3&)");
}

DirectionalLight::DirectionalLight()
    : Light(), direction_vector_(0.0F, 0.0F, 0.0F) {
  LOG_TRACE("DirectionalLight()");
}

DirectionalLight::DirectionalLight(const glm::vec3& direction)
    : Light(), direction_vector_(direction) {
  LOG_TRACE("DirectionalLight(const glm::vec3&)");
}

DirectionalLight::DirectionalLight(const glm::vec3& color,
                                   const glm::vec3& intensity,
                                   const glm::vec3& direction)
    : Light(color, intensity), direction_vector_(direction) {
  //
}

void DirectionalLight::direction(const glm::vec3& in) {
  direction_vector_ = in;
}

const glm::vec3& DirectionalLight::direction() const {
  return direction_vector_;
}

float* DirectionalLight::direction() {
  return glm::value_ptr(direction_vector_);
}
