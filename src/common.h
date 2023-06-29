#ifndef COMMON_H
#define COMMON_H

#include "glm/glm.hpp"

/**
 * Struttura dati che contiene gli attributi di un vertice.
 */
struct Vertex {
  glm::vec3 position;   // Coordinate spaziali
  glm::vec3 normal;     // Normale
  glm::vec2 textcoord;  // Coordinate di texture

  Vertex();

  Vertex(float x, float y, float z, float xn, float yn, float zn, float s,
         float t);

  Vertex(const glm::vec3& xyz, const glm::vec3& norm, const glm::vec2& txt);
};

enum ATTRIB_ID {
  ATTRIB_POSITIONS = 0,
  ATTRIB_NORMALS = 1,
  ATTRIB_COLOR_TEXTURE_COORDS = 2
};

enum TEXTURE_UNIT_ID { TEXTURE_COLOR = 0 };

#endif  // COMMON_H