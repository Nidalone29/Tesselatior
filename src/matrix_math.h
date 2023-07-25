#ifndef MATRIX_MATH_H
#define MATRIX_MATH_H

#include <glm/glm.hpp>

// because some compilers does not have it
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

namespace Math {

const constexpr float g2r = M_PI / 180.0;
const constexpr float r2g = 180.0 / M_PI;

static constexpr float to_radiant(float deg);
static constexpr float to_degree(float rad);

/**
 * Funzione statica che calcola la matrice di traslazione sui tre assi.
 *
 * @param x offsetX di traslazione rispetto all'asse X
 * @param y offsetY di traslazione rispetto all'asse Y
 * @param z offsetZ di traslazione rispetto all'asse Z
 * @return la matrice 4x4 di traslazione
 */
static constexpr glm::mat4 translationMatrix(float offsetX, float offsetY,
                                             float offsetZ);

/**
 * Funzione statica che calcola la matrice di scaling sui tre assi.
 *
 * @param factorX fattore di scaling rispetto all'asse X
 * @param factorY fattore di scaling rispetto all'asse Y
 * @param factorZ fattore di scaling rispetto all'asse Z
 * @return la matrice 4x4 di scaling
 */
static constexpr glm::mat4 scalingMatrix(float factorX, float factorY,
                                         float factorZ);

/**
 * Funzione statica che calcola la matrice di rotazione dati tre angoli
 * in gradi.
 *
 * @param degX angolo di rotazione rispetto all'asse X
 * @param degY angolo di rotazione rispetto all'asse Y
 * @param degZ angolo di rotazione rispetto all'asse Z
 * @return la matrice 4x4 di rotazione
 */
static constexpr glm::mat4 rotationMatrix(float degX, float degY, float degZ);
/**
 * Funzione statica che calcola la matrice di rotazione intorno ad
 * un vettore di un dato angolo (in gradi).
 * La trasfromazione unsa la formula di Rodriguez.
 *
 * @param deg angolo di rotazione in gradi
 * @param axis vettore che contiene l'asse di rotazione
 * @return la matrice 3x3 di rotazione
 */
static glm::mat3 rotation3Matrix(float deg, const glm::vec3& axis);

}  // namespace Math

#endif  // MATRIX_MATH_H