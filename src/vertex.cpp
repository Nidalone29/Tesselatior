#include "vertex.h"

Vertex::Vertex(float x, float y, float z, float xn, float yn, float zn, float s,
               float t)
    : position(x, y, z),
      normal(xn, yn, zn),
      text_coords(s, t),
      halfedge(nullptr) {
  //
}

Vertex::Vertex(const glm::vec3& xyz, const glm::vec3& norm,
               const glm::vec2& txt)
    : position(xyz), normal(norm), text_coords(txt), halfedge(nullptr) {
  //
}

bool Vertex::IsEven() const {
  return Valence() % 2 == 0;
}

bool Vertex::IsOdd() const {
  return !IsEven();
}

int Vertex::Valence() const {
  const HalfEdge* curr = halfedge->twin->next;
  int counter = 0;
  while (curr != halfedge) {
    counter++;
    curr = curr->twin->next;
  }
  return counter;
}
