#include "vertex.h"

#include "../logger.h"

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

// this is for initializing a vertex that will later have its normal recomputed
Vertex::Vertex(const glm::vec3& xyz, const glm::vec2& txt)
    : Vertex(xyz, {0.0F, 0.0F, 0.0F}, txt) {
  //
}

Vertex::Vertex(const Vertex& other)
    : position(other.position),
      normal(other.normal),
      text_coords(other.text_coords),
      halfedge(nullptr) {
  //
}

Vertex& Vertex::operator=(const Vertex& other) {
  if (this != &other) {
    Vertex temp(other);
    std::swap(this->position, temp.position);
    std::swap(this->normal, temp.normal);
    std::swap(this->text_coords, temp.text_coords);
    halfedge = nullptr;
  }

  return *this;
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

HalfEdge::HalfEdge(Face* f)
    : next(nullptr), twin(nullptr), vert(nullptr), face(f), edge(nullptr) {
  //
}

bool HalfEdge::IsBoundary() const {
  return (twin == nullptr);
}

HalfEdge* HalfEdge::Previous() const {
  HalfEdge* curr = next;
  while (curr->next != this) {
    curr = curr->next;
  }
  return curr;
}

// https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal#Newell's_Method
// using Newell's method this works on an arbitrary face
// NOTE: the face normal here is NOT normalized because when you add it up to
// vertex, the contribution given by the area (result of the cross product)
// results in a better quality normal as explained in
// https://iquilezles.org/articles/normals/
glm::vec3 Face::ComputeNormalWithArea() const {
  glm::vec3 normal = {0.0F, 0.0F, 0.0F};
  const HalfEdge* start = halfedge;
  HalfEdge* curr = halfedge;

  do {
    const glm::vec3& current_v = curr->vert->position;
    const glm::vec3& next_v = curr->next->vert->position;
    normal.x += (current_v.y - next_v.y) * (current_v.z + next_v.z);
    normal.y += (current_v.z - next_v.z) * (current_v.x + next_v.x);
    normal.z += (current_v.x - next_v.x) * (current_v.y + next_v.y);

    curr = curr->next;
  } while (start != curr);

  return normal;
}
