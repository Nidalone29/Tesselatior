#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>

#include "matrix_math.h"

class Transform {
 public:
  Transform();
  Transform(const glm::mat4 scale, const glm::mat4 rotation,
            const glm::mat4 translation);

  ~Transform() = default;
  Transform(const Transform& other) = default;
  Transform& operator=(const Transform& other) = default;
  Transform(Transform&& other) = default;
  Transform& operator=(Transform&& other) = default;

  /**
   * @brief Applica la rotazione alla matrice corrente. Gli angoli sono ingradi.
   *
   * @param degX angolo di rotazione rispetto all'asse X
   * @param degY angolo di rotazione rispetto all'asse Y
   * @param degZ angolo di rotazione rispetto all'asse Z
   */
  void rotate(float degX, float degY, float degZ);

  /**
   * Applica la rotazione alla matrice corrente. Gli angoli sono in gradi.
   * @param angles vettore che contiene i 3 angoli di rotazione
   */
  void rotate(const glm::vec3& angles);

  /**
   * Applica la traslazione alla matrice corrente.
   * @param x offset di traslazione rispetto all'asse X
   * @param y offset di traslazione rispetto all'asse Y
   * @param z offset di traslazione rispetto all'asse Z
   */
  void translate(float x, float y, float z);

  /**
   * Applica la traslazione alla matrice corrente.
   * @param offset vettore che contiene gli offset di traslazione
   */
  void translate(const glm::vec3& offset);

  /**
   * Applica lo scaling alla matrice corrente.
   * @param sx fattore di scaling rispetto all'asse X
   * @param sy fattore di scaling rispetto all'asse Y
   * @param sz fattore di scaling rispetto all'asse Z
   */
  void scale(float sx, float sy, float sz);

  /**
   * Applica lo scaling uniforme alla matrice corrente.
   * @param sc valore di scaling
   */
  void scale(float sc);

  /**
   * Applica lo scaling alla matrice corrente.
   * @param factor vettore con i tre valori di scaling
   */
  void scale(const glm::vec3& factor);

  /**
   * @brief expects a matrix in homogeneous coordinates (rotation, scaling or
   * translation)
   *
   * @param matrix
   * @return
   */
  Transform operator*(const glm::mat4& matrix);

  const glm::mat4& matrix() const;

 private:
  void Update();

  glm::mat4 transform_;

  glm::mat4 rotation_;
  glm::mat4 translation_;
  glm::mat4 scaling_;
};

#endif  // TRANSFORM_H
