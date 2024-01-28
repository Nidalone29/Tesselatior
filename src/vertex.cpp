#include "vertex.h"

Vertex::Vertex(float x, float y, float z, float xn, float yn, float zn, float s,
               float t)
    : _position(x, y, z), _normal(xn, yn, zn), _text_coords(s, t) {
  //
}

Vertex::Vertex(const glm::vec3& xyz, const glm::vec3& norm,
               const glm::vec2& txt)
    : _position(xyz), _normal(norm), _text_coords(txt) {
  //
}
