#include "common.h"

Vertex::Vertex() : position(0,0,0), normal(0,0,0), textcoord(0,0) {}

Vertex::Vertex(
  float x, float y, float z, 
  float xn, float yn, float zn,
  float s, float t) :position(x,y,z), normal(xn,yn,zn), textcoord(s,t) {}


Vertex::Vertex(const glm::vec3 &xyz, const glm::vec3 &norm, const glm::vec2 &txt) 
  : position(xyz), normal(norm), textcoord(txt) {}


