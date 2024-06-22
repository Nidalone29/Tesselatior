#include "vertex.h"

#include "logger.h"

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

// this only works for non boundary vertices
int Vertex::Valence() const {
  // kinda suboptimal because I am iterating around the edge two times but
  // whatever TODO optimize
  if (IsBoundary()) {
    LOG_ERROR(
        "INVALID VERTEX OPERATION (CAN'T COMPUTE VALENCE OF BOUNDARY VERTEX)");
    throw;  // invalid vertex operation!
  }

  const HalfEdge* curr = halfedge->twin->next;
  int counter = 1;
  while (curr != halfedge) {
    counter++;
    curr = curr->twin->next;
  }
  return counter;
}

bool Vertex::IsBoundary() const {
  if (halfedge->IsBoundary()) {
    return true;
  }
  const HalfEdge* curr = halfedge->twin->next;
  while (curr != halfedge) {
    if (curr->IsBoundary()) {
      return true;
    }
    curr = curr->twin->next;
  }

  return false;
}
