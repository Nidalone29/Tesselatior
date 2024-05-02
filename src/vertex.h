#ifndef VERTEX_H
#define VERTEX_H

#include <vector>
#include <glm/glm.hpp>

struct Edge;
struct Face;
struct HalfEdge;

struct Vertex {
  glm::vec3 position;     // Coordinate spaziali
  glm::vec3 normal;       // Vertex normal (for phong shading)
  glm::vec2 text_coords;  // Coordinate di texture

  HalfEdge* edge;  // one of it's outgoing halfedges

  Vertex(float x, float y, float z, float xn, float yn, float zn, float s,
         float t);

  Vertex(const glm::vec3& xyz, const glm::vec3& norm, const glm::vec2& txt);
};

struct HalfEdge {
  HalfEdge* next;
  HalfEdge* twin;  // opposite
  Vertex* vert;    // The vertex that the halfedge points to
  Edge* edge;
  Face* face;
};

struct Face {
  HalfEdge* halfedge;
};

#endif  // VERTEX_H
