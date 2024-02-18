#include "transform.h"

Transform::Transform() {
  transform_ = glm::mat4(1.0F);
}

Transform::Transform(const glm::mat4 scale, const glm::mat4 rotation,
                     const glm::mat4 translation) {
  transform_ = translation * rotation * scale;
}

/**
 * @brief expects a matrix in homogeneous coordinates (rotation, scaling or
 * translation) (calculated maybe using Math.cpp)
 * TODO enforce the type of matrix
 * @param matrix
 */
Transform Transform::operator*(const glm::mat4& matrix) {
  transform_ *= matrix;
  return *this;
}

const glm::mat4& Transform::matrix() const {
  return transform_;
}
