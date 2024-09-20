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
  Vertex(const glm::vec3& xyz, const glm::vec2& txt);
  Vertex(const Vertex& other);
  Vertex& operator=(const Vertex& other);
  // TODO properly support move semantics
  Vertex(Vertex&& other) = default;
  Vertex& operator=(Vertex&& other) = default;

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

  HalfEdge() = default;
  explicit HalfEdge(Face* f);

  [[nodiscard]] bool IsBoundary() const;
  [[nodiscard]] HalfEdge* Previous() const;
};

// with one of the halfedge we iterate in halfedge->next (3/4 times) for the
// vertices
struct Face {
  HalfEdge* halfedge;

  [[nodiscard]] glm::vec3 ComputeNormalWithArea() const;
};

struct Edge {
  HalfEdge* halfedge;
};

#endif  // VERTEX_H
