#include "transform.h"

#include "matrix_math.h"

Transform::Transform() {
  scaling_ = translation_ = rotation_ = transform_ = glm::mat4(1.0F);
}

Transform::Transform(const glm::mat4 scale, const glm::mat4 rotation,
                     const glm::mat4 translation)
    : rotation_(rotation), translation_(translation), scaling_(scale) {
  Update();
}

void Transform::rotate(float degX, float degY, float degZ) {
  rotation_ = Math::rotationMatrix(degX, degY, degZ);
  Update();
}

void Transform::rotate(const glm::vec3& angles) {
  rotate(angles.x, angles.y, angles.z);
  Update();
}

void Transform::translate(float x, float y, float z) {
  translation_ = Math::translationMatrix(x, y, z);
  Update();
}

void Transform::translate(const glm::vec3& offset) {
  translate(offset.x, offset.y, offset.z);
}

void Transform::scale(float sx, float sy, float sz) {
  scaling_ = Math::translationMatrix(sx, sy, sz);
  Update();
}

void Transform::scale(float sc) {
  scale(sc, sc, sc);
}

void Transform::scale(const glm::vec3& factor) {
  scale(factor.x, factor.y, factor.z);
}

const glm::mat4& Transform::matrix() const {
  return transform_;
}

void Transform::Update() {
  transform_ = translation_ * rotation_ * scaling_;
}
