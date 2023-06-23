#include <iostream>
#include "transform.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

std::ostream& operator<<(std::ostream& os, const glm::mat4& m) {
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 4; ++x) {
      os << m[x][y] << " ";
    }
    os << std::endl;
  }

  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec4& v) {
  for (int y = 0; y < 4; ++y) {
    os << v[y] << " ";
  }

  return os;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
  for (int y = 0; y < 3; ++y) {
    os << v[y] << " ";
  }

  return os;
}

namespace {
const float g2r = M_PI / 180.0;
const float r2g = 180.0 / M_PI;
}  // namespace

float to_radiant(float deg) { return deg * ::g2r; }

float to_degree(float rad) { return rad * ::r2g; }

LocalTransform::LocalTransform() { reset(); }

void LocalTransform::update() {
  _combined = _translation * _rotation * _scaling;
}

void LocalTransform::rotate(float degX, float degY, float degZ) {
  _rotation = rotation(degX, degY, degZ);

  update();
}

void LocalTransform::rotate(const glm::vec3& angle) {
  rotate(angle.x, angle.y, angle.z);
}

void LocalTransform::translate(float x, float y, float z) {
  _translation = translation(x, y, z);

  update();
}

void LocalTransform::translate(const glm::vec3& offset) {
  translate(offset.x, offset.y, offset.z);
}

void LocalTransform::scale(float sx, float sy, float sz) {
  _scaling = scaling(sx, sy, sz);

  update();
}

void LocalTransform::scale(float sc) { scale(sc, sc, sc); }

void LocalTransform::scale(const glm::vec3& factor) {
  scale(factor.x, factor.y, factor.z);
}

const glm::mat4& LocalTransform::T() const { return _combined; }

void LocalTransform::reset() {
  _scaling = _translation = _rotation = _combined = glm::mat4(1.0f);
}

glm::mat4 LocalTransform::rotation(float degX, float degY, float degZ) {
  glm::mat4 rx(1.0f), ry(1.0f), rz(1.0f);

  float thetaX = to_radiant(degX);
  float thetaY = to_radiant(degY);
  float thetaZ = to_radiant(degZ);

  rx[1][1] = cosf(thetaX);
  rx[2][1] = -sinf(thetaX);
  rx[1][2] = sinf(thetaX);
  rx[2][2] = cosf(thetaX);

  ry[0][0] = cosf(thetaY);
  ry[2][0] = sinf(thetaY);
  ry[0][2] = -sinf(thetaY);
  ry[2][2] = cosf(thetaY);

  rz[0][0] = cosf(thetaZ);
  rz[1][0] = -sinf(thetaZ);
  rz[0][1] = sinf(thetaZ);
  rz[1][1] = cosf(thetaZ);

  return rz * ry * rx;
}

glm::mat4 LocalTransform::translation(float offsetX, float offsetY,
                                      float offsetZ) {
  glm::mat4 t(1.0f);
  t[3] = glm::vec4(offsetX, offsetY, offsetZ, 1.0f);
  return t;
}

glm::mat4 LocalTransform::scaling(float factorX, float factorY, float factorZ) {
  assert(factorX > 0);
  assert(factorY > 0);
  assert(factorZ > 0);

  glm::mat4 s(1.0f);
  s[0][0] = factorX;
  s[1][1] = factorY;
  s[2][2] = factorZ;
  return s;
}

glm::mat3 LocalTransform::rotation3(float deg, const glm::vec3& axis) {
  glm::vec3 n = glm::normalize(axis);

  glm::mat3 out(1.0f);

  float t = to_radiant(deg);

  float s = sin(t);
  float c = cos(t);
  float c1 = 1.0f - c;

  out[0][0] = n.x * n.x + (1.0f - n.x * n.x) * c;
  out[0][1] = n.x * n.y * c1 + n.z * s;
  out[0][2] = n.x * n.z * c1 - n.y * s;

  out[1][0] = n.x * n.y * c1 - n.z * s;
  out[1][1] = n.y * n.y + (1.0f - n.y * n.y) * c;
  out[1][2] = n.y * n.z * c1 + n.x * s;

  out[2][0] = n.x * n.z * c1 + n.y * s;
  out[2][1] = n.y * n.z * c1 - n.x * s;
  out[2][2] = n.z * n.z + (1.0f - n.z * n.z) * c;

  return out;
}
