#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

// struct for having sequential access to the data
// TODO check if c++ classes do the same because i don't remember
struct Vertex {
  glm::vec3 _position;     // Coordinate spaziali
  glm::vec3 _normal;       // Vertex normal (for phong shading)
  glm::vec2 _text_coords;  // Coordinate di texture
};

#endif  // VERTEX_H