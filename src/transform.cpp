#include "transform.h"

Transform::Transform() {
  _transform = glm::mat4(1.0F);
}

Transform::Transform(const glm::mat4 scale, const glm::mat4 rotation,
                     const glm::mat4 translation) {
  _transform = translation * rotation * scale;
}

/**
 * @brief expects a matrix in homogeneous coordinates (rotation, scaling or
 * translation) (calculated maybe using Math.cpp)
 * TODO enforce the type of matrix
 * @param matrix
 */
Transform Transform::operator*(const glm::mat4 matrix) {
  _transform *= matrix;
  return *this;
}

const glm::mat4& Transform::getMatrix() const {
  return _transform;
}
