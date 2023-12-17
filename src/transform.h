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

  Transform operator*(const glm::mat4 matrix);

  const glm::mat4& getMatrix() const;

 private:
  glm::mat4 _transform;
};

#endif  // TRANSFORM_H
