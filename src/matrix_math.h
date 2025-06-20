#ifndef MATRIX_MATH_H
#define MATRIX_MATH_H

// for PI
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/math-constants?view=msvc-170
#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/glm.hpp>

namespace Math {

constexpr float g2r = M_PI / 180.0;
constexpr float r2g = 180.0 / M_PI;

static constexpr float to_radiant(const float deg) {
  return deg * g2r;
}

static constexpr float to_degree(const float rad) {
  return rad * r2g;
}

/**
 * Funzione statica che calcola la matrice di traslazione sui tre assi.
 *
 * @param x offsetX di traslazione rispetto all'asse X
 * @param y offsetY di traslazione rispetto all'asse Y
 * @param z offsetZ di traslazione rispetto all'asse Z
 * @return la matrice 4x4 di traslazione
 */
static glm::mat4 translationMatrix(const float offsetX, const float offsetY,
                                   const float offsetZ) {
  glm::mat4 t(1.0F);
  t[3] = glm::vec4(offsetX, offsetY, offsetZ, 1.0F);
  return t;
}

/**
 * Funzione statica che calcola la matrice di scaling sui tre assi.
 *
 * @param factorX fattore di scaling rispetto all'asse X
 * @param factorY fattore di scaling rispetto all'asse Y
 * @param factorZ fattore di scaling rispetto all'asse Z
 * @return la matrice 4x4 di scaling
 */
static glm::mat4 scalingMatrix(const float factorX, const float factorY,
                               const float factorZ) {
  assert(factorX > 0);
  assert(factorY > 0);
  assert(factorZ > 0);

  glm::mat4 s(1.0F);
  s[0][0] = factorX;
  s[1][1] = factorY;
  s[2][2] = factorZ;
  return s;
}
/**
 * Funzione statica che calcola la matrice di rotazione dati tre angoli
 * in gradi.
 *
 * @param degX angolo di rotazione rispetto all'asse X
 * @param degY angolo di rotazione rispetto all'asse Y
 * @param degZ angolo di rotazione rispetto all'asse Z
 * @return la matrice 4x4 di rotazione
 */
static glm::mat4 rotationMatrix(const float degX, const float degY,
                                const float degZ) {
  glm::mat4 rx(1.0F), ry(1.0F), rz(1.0F);

  const float thetaX = to_radiant(degX);
  const float thetaY = to_radiant(degY);
  const float thetaZ = to_radiant(degZ);

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

  glm::mat4 result(1.0F);
  result *= rz;
  result *= ry;
  result *= rx;

  return result;
}

/**
 * Funzione statica che calcola la matrice di rotazione intorno ad
 * un vettore di un dato angolo (in gradi).
 * La trasfromazione unsa la formula di Rodriguez.
 *
 * @param deg angolo di rotazione in gradi
 * @param axis vettore che contiene l'asse di rotazione
 * @return la matrice 3x3 di rotazione
 */
static glm::mat3 rotation3Matrix(const float deg, const glm::vec3& axis) {
  const glm::vec3 n = glm::normalize(axis);

  glm::mat3 out(1.0F);

  const float t = to_radiant(deg);

  const float sine = sin(t);
  const float cosine = cos(t);
  const float acosine = 1.0F - cosine;

  out[0][0] = n.x * n.x + (1.0F - n.x * n.x) * cosine;
  out[0][1] = n.x * n.y * acosine + n.z * sine;
  out[0][2] = n.x * n.z * acosine - n.y * sine;

  out[1][0] = n.x * n.y * acosine - n.z * sine;
  out[1][1] = n.y * n.y + (1.0F - n.y * n.y) * cosine;
  out[1][2] = n.y * n.z * acosine + n.x * sine;

  out[2][0] = n.x * n.z * acosine + n.y * sine;
  out[2][1] = n.y * n.z * acosine - n.x * sine;
  out[2][2] = n.z * n.z + (1.0F - n.z * n.z) * cosine;

  return out;
}

}  // namespace Math

#endif  // MATRIX_MATH_H
