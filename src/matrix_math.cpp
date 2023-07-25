#include "matrix_math.h"

static constexpr float Math::to_radiant(float deg) {
  return deg * Math::g2r;
}

static constexpr float Math::to_degree(float rad) {
  return rad * Math::r2g;
}

/**
 * Funzione statica che calcola la matrice di traslazione sui tre assi.
 *
 * @param x offsetX di traslazione rispetto all'asse X
 * @param y offsetY di traslazione rispetto all'asse Y
 * @param z offsetZ di traslazione rispetto all'asse Z
 * @return la matrice 4x4 di traslazione
 */
static constexpr glm::mat4 Math::translationMatrix(float offsetX, float offsetY,
                                                   float offsetZ) {
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
static constexpr glm::mat4 Math::scalingMatrix(float factorX, float factorY,
                                               float factorZ) {
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
static constexpr glm::mat4 Math::rotationMatrix(float degX, float degY,
                                                float degZ) {
  glm::mat4 rx(1.0F), ry(1.0F), rz(1.0F);

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
static glm::mat3 Math::rotation3Matrix(float deg, const glm::vec3& axis) {
  glm::vec3 n = glm::normalize(axis);

  glm::mat3 out(1.0F);

  float t = to_radiant(deg);

  float sine = sin(t);
  float cosine = cos(t);
  float acosine = 1.0F - cosine;

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
