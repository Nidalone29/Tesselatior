#include "cube.h" 0Ff
#include "common.h"
#include "glm/glm.hpp"

#include <iostream>

Cube::Cube() : _initialized(false) {}

void Cube::init() {
  Vertex Vertices[36] = {

      // Davanti
      Vertex(glm::vec3(-1.0F, -1.0F, 1.0F), glm::vec3(0, 0, 1),
             glm::vec2(0.25, 0.5)),
      Vertex(glm::vec3(1.0F, -1.0F, 1.0F), glm::vec3(0, 0, 1),
             glm::vec2(0.5, 0.5)),
      Vertex(glm::vec3(-1.0F, 1.0F, 1.0F), glm::vec3(0, 0, 1),
             glm::vec2(0.25, 0.75)),
      Vertex(glm::vec3(1.0F, -1.0F, 1.0F), glm::vec3(0, 0, 1),
             glm::vec2(0.5, 0.5)),
      Vertex(glm::vec3(1.0F, 1.0F, 1.0F), glm::vec3(0, 0, 1),
             glm::vec2(0.5, 0.75)),
      Vertex(glm::vec3(-1.0F, 1.0F, 1.0F), glm::vec3(0, 0, 1),
             glm::vec2(0.25, 0.75)),

      // Destra
      Vertex(glm::vec3(1.0F, -1.0F, 1.0F), glm::vec3(1, 0, 0),
             glm::vec2(0.5, 0.5)),
      Vertex(glm::vec3(1.0F, -1.0F, -1.0F), glm::vec3(1, 0, 0),
             glm::vec2(0.75, 0.5)),
      Vertex(glm::vec3(1.0F, 1.0F, 1.0F), glm::vec3(1, 0, 0),
             glm::vec2(0.5, 0.75)),
      Vertex(glm::vec3(1.0F, -1.0F, -1.0F), glm::vec3(1, 0, 0),
             glm::vec2(0.75, 0.5)),
      Vertex(glm::vec3(1.0F, 1.0F, -1.0F), glm::vec3(1, 0, 0),
             glm::vec2(0.75, 0.75)),
      Vertex(glm::vec3(1.0F, 1.0F, 1.0F), glm::vec3(1, 0, 0),
             glm::vec2(0.5, 0.75)),

      // Alto
      Vertex(glm::vec3(-1.0F, 1.0F, 1.0F), glm::vec3(0, 1, 0),
             glm::vec2(0.25, 0.75)),
      Vertex(glm::vec3(1.0F, 1.0F, 1.0F), glm::vec3(0, 1, 0),
             glm::vec2(0.5, 0.75)),
      Vertex(glm::vec3(-1.0F, 1.0F, -1.0F), glm::vec3(0, 1, 0),
             glm::vec2(0.25, 1)),
      Vertex(glm::vec3(1.0F, 1.0F, 1.0F), glm::vec3(0, 1, 0),
             glm::vec2(0.5, 0.75)),
      Vertex(glm::vec3(1.0F, 1.0F, -1.0F), glm::vec3(0, 1, 0),
             glm::vec2(0.5, 1)),
      Vertex(glm::vec3(-1.0F, 1.0F, -1.0F), glm::vec3(0, 1, 0),
             glm::vec2(0.25, 1)),

      // Sinistra
      Vertex(glm::vec3(-1.0F, -1.0F, 1.0F), glm::vec3(-1, 0, 0),
             glm::vec2(0.25, 0.5)),
      Vertex(glm::vec3(-1.0F, 1.0F, 1.0F), glm::vec3(-1, 0, 0),
             glm::vec2(0.25, 0.75)),
      Vertex(glm::vec3(-1.0F, -1.0F, -1.0F), glm::vec3(-1, 0, 0),
             glm::vec2(0, 0.5)),
      Vertex(glm::vec3(-1.0F, -1.0F, -1.0F), glm::vec3(-1, 0, 0),
             glm::vec2(0, 0.5)),
      Vertex(glm::vec3(-1.0F, 1.0F, 1.0F), glm::vec3(-1, 0, 0),
             glm::vec2(0.25, 0.75)),
      Vertex(glm::vec3(-1.0F, 1.0F, -1.0F), glm::vec3(-1, 0, 0),
             glm::vec2(0, 0.75)),

      // Basso
      Vertex(glm::vec3(-1.0F, -1.0F, 1.0F), glm::vec3(0, -1, 0),
             glm::vec2(0.25, 0.5)),
      Vertex(glm::vec3(-1.0F, -1.0F, -1.0F), glm::vec3(0, -1, 0),
             glm::vec2(0.25, 0.25)),
      Vertex(glm::vec3(1.0F, -1.0F, 1.0F), glm::vec3(0, -1, 0),
             glm::vec2(0.5, 0.5)),
      Vertex(glm::vec3(1.0F, -1.0F, 1.0F), glm::vec3(0, -1, 0),
             glm::vec2(0.5, 0.5)),
      Vertex(glm::vec3(-1.0F, -1.0F, -1.0F), glm::vec3(0, -1, 0),
             glm::vec2(0.25, 0.25)),
      Vertex(glm::vec3(1.0F, -1.0F, -1.0F), glm::vec3(0, -1, 0),
             glm::vec2(0.5, 0.25)),

      // Dietro
      Vertex(glm::vec3(-1.0F, -1.0F, -1.0F), glm::vec3(0, 0, -1),
             glm::vec2(1, 0.5)),
      Vertex(glm::vec3(-1.0F, 1.0F, -1.0F), glm::vec3(0, 0, -1),
             glm::vec2(1, 0.75)),
      Vertex(glm::vec3(1.0F, -1.0F, -1.0F), glm::vec3(0, 0, -1),
             glm::vec2(0.75, 0.5)),
      Vertex(glm::vec3(1.0F, -1.0F, -1.0F), glm::vec3(0, 0, -1),
             glm::vec2(0.75, 0.5)),
      Vertex(glm::vec3(-1.0F, 1.0F, -1.0F), glm::vec3(0, 0, -1),
             glm::vec2(1, 0.75)),
      Vertex(glm::vec3(1.0F, 1.0F, -1.0F), glm::vec3(0, 0, -1),
             glm::vec2(0.75, 0.75))};

  glGenVertexArrays(1, &(_VAO));
  glBindVertexArray(_VAO);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(
      ATTRIB_POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      reinterpret_cast<GLvoid*>(offsetof(struct Vertex, position)));

  glVertexAttribPointer(
      ATTRIB_NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      reinterpret_cast<GLvoid*>(offsetof(struct Vertex, normal)));

  glVertexAttribPointer(
      ATTRIB_COLOR_TEXTURE_COORDS, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      reinterpret_cast<GLvoid*>(offsetof(struct Vertex, textcoord)));

  _initialized = true;

  glBindVertexArray(0);
  // glBindBuffer(GL_ARRAY_BUFFER, 0); // TODO CHECK

  _color_texture.load("cube.png");
}

void Cube::render() {
  if (!_initialized) {
    init();
  }

  glBindVertexArray(_VAO);

  glEnableVertexAttribArray(ATTRIB_POSITIONS);
  glEnableVertexAttribArray(ATTRIB_NORMALS);
  glEnableVertexAttribArray(ATTRIB_COLOR_TEXTURE_COORDS);

  _color_texture.bind(TEXTURE_COLOR);

  glDrawArrays(GL_TRIANGLES, 0, 36);

  glBindVertexArray(0);
}
