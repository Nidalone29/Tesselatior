#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

struct HalfEdge;

struct Vertex {
  glm::vec3 position;     // Coordinate spaziali
  glm::vec3 normal;       // Vertex normal (for phong shading)
  glm::vec2 text_coords;  // Coordinate di texture
  HalfEdge* edge;

  Vertex(float x, float y, float z, float xn, float yn, float zn, float s,
         float t);

  Vertex(const glm::vec3& xyz, const glm::vec3& norm, const glm::vec2& txt);
};

struct HalfEdge {
  Vertex* vert;
  HalfEdge* prev;
  HalfEdge* next;
  HalfEdge* opposite;  // twin
};

#endif  // VERTEX_H
