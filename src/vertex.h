#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

struct Face;
struct HalfEdge;

struct Vertex {
  glm::vec3 position;     // spatial coordinates
  glm::vec3 normal;       // vertex normal (for phong shading)
  glm::vec2 text_coords;  // texture coordinates

  HalfEdge* halfedge;  // one of it's outgoing halfedge

  Vertex(float x, float y, float z, float xn, float yn, float zn, float s,
         float t);

  Vertex(const glm::vec3& xyz, const glm::vec3& norm, const glm::vec2& txt);
};

struct HalfEdge {
  HalfEdge* next;
  HalfEdge* twin;  // opposite
  Vertex* vert;    // The vertex that the halfedge points to
  Face* face;

  explicit HalfEdge(Face* f)
      : next(nullptr), twin(nullptr), vert(nullptr), face(f) {
    //
  }

  [[nodiscard]] bool IsBoundary() const {
    return (twin == nullptr);
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
