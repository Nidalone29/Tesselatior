#ifndef VERTEX_H
#define VERTEX_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

struct Edge;
struct Face;
struct HalfEdge;

struct Vertex {
  glm::vec3 position;     // spatial coordinates
  glm::vec3 normal;       // vertex normal (for phong shading)
  glm::vec2 text_coords;  // texture coordinates

  HalfEdge* halfedge;  // one of it's outgoing halfedge

  Vertex() = default;
  Vertex(float x, float y, float z, float xn, float yn, float zn, float s,
         float t);

  Vertex(const glm::vec3& xyz, const glm::vec3& norm, const glm::vec2& txt);

  [[nodiscard]] bool IsEven() const;
  [[nodiscard]] bool IsOdd() const;
  [[nodiscard]] int Valence() const;
  [[nodiscard]] bool IsBoundary() const;
};

struct HalfEdge {
  HalfEdge* next;
  HalfEdge* twin;  // opposite
  Vertex* vert;    // The vertex that the halfedge points to
  // this halfedge belongs to a face and an edge
  Face* face;
  Edge* edge;

  explicit HalfEdge(Face* f)
      : next(nullptr), twin(nullptr), vert(nullptr), face(f), edge(nullptr) {
    //
  }

  [[nodiscard]] bool IsBoundary() const {
    return (twin == nullptr);
  }

  [[nodiscard]] HalfEdge* Previous() const {
    HalfEdge* curr = next;
    while (curr->next != this) {
      curr = curr->next;
    }
    return curr;
  }
};

// with one of the halfedge we iterate in halfedge->next (3/4 times) for the
// vertices
struct Face {
  HalfEdge* halfedge;
};

struct Edge {
  HalfEdge* halfedge;
};

#endif  // VERTEX_H
