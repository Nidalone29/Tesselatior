#include "vertex.h"

Vertex::Vertex(float x, float y, float z, float xn, float yn, float zn, float s,
               float t)
    : position(x, y, z), normal(xn, yn, zn), text_coords(s, t) {
  //
}

Vertex::Vertex(const glm::vec3& xyz, const glm::vec3& norm,
               const glm::vec2& txt)
    : position(xyz), normal(norm), text_coords(txt) {
  //
}
