#include "octahedron.h"
#include "common.h"
#include "glm/glm.hpp"

#include <iostream>

Octahedron::Octahedron() : _initialized(false) {}

void Octahedron::init(void) {
  Vertex Vertices[24] = {// Alto Davanti
                         Vertex(glm::vec3(-1.0f, 0.0f, 1.0f),
                                glm::vec3(0, 0.71, 0.71), glm::vec2(0, 0)),
                         Vertex(glm::vec3(1.0f, 0.0f, 1.0f),
                                glm::vec3(0, 0.71, 0.71), glm::vec2(1, 0)),
                         Vertex(glm::vec3(0.0f, 1.0f, 0.0f),
                                glm::vec3(0, 0.71, 0.71), glm::vec2(0.5, 1)),

                         // Alto Destra
                         Vertex(glm::vec3(1.0f, 0.0f, 1.0f),
                                glm::vec3(0.71, 0.71, 0), glm::vec2(0, 0)),
                         Vertex(glm::vec3(1.0f, 0.0f, -1.0f),
                                glm::vec3(0.71, 0.71, 0), glm::vec2(1, 0)),
                         Vertex(glm::vec3(0.0f, 1.0f, 0.0f),
                                glm::vec3(0.71, 0.71, 0), glm::vec2(0.5, 1)),

                         // Alto Sinistra
                         Vertex(glm::vec3(-1.0f, 0.0f, -1.0f),
                                glm::vec3(-0.71, 0.71, 0), glm::vec2(0, 0)),
                         Vertex(glm::vec3(-1.0f, 0.0f, 1.0f),
                                glm::vec3(-0.71, 0.71, 0), glm::vec2(1, 0)),
                         Vertex(glm::vec3(0.0f, 1.0f, 0.0f),
                                glm::vec3(-0.71, 0.71, 0), glm::vec2(0.5, 1)),

                         // Alto Dietro
                         Vertex(glm::vec3(-1.0f, 0.0f, -1.0f),
                                glm::vec3(0, 0.71, -0.71), glm::vec2(1, 0)),
                         Vertex(glm::vec3(0.0f, 1.0f, 0.0f),
                                glm::vec3(0, 0.71, -0.71), glm::vec2(0.5, 1)),
                         Vertex(glm::vec3(1.0f, 0.0f, -1.0f),
                                glm::vec3(0, 0.71, -0.71), glm::vec2(0, 0)),

                         // Basso Davanti
                         Vertex(glm::vec3(-1.0f, 0.0f, 1.0f),
                                glm::vec3(0, -0.71, 0.71), glm::vec2(0, 1)),
                         Vertex(glm::vec3(0.0f, -1.0f, 0.0f),
                                glm::vec3(0, -0.71, 0.71), glm::vec2(0.5, 0)),
                         Vertex(glm::vec3(1.0f, 0.0f, 1.0f),
                                glm::vec3(0, -0.71, 0.71), glm::vec2(1, 1)),

                         // Basso Destra
                         Vertex(glm::vec3(1.0f, 0.0f, 1.0f),
                                glm::vec3(0.71, -0.71, 0), glm::vec2(0, 1)),
                         Vertex(glm::vec3(0.0f, -1.0f, 0.0f),
                                glm::vec3(0.71, -0.71, 0), glm::vec2(0.5, 0)),
                         Vertex(glm::vec3(1.0f, 0.0f, -1.0f),
                                glm::vec3(0.71, -0.71, 0), glm::vec2(1, 1)),

                         // Basso Sinistra
                         Vertex(glm::vec3(-1.0f, 0.0f, -1.0f),
                                glm::vec3(-0.71, -0.71, 0), glm::vec2(0, 1)),
                         Vertex(glm::vec3(0.0f, -1.0f, 0.0f),
                                glm::vec3(-0.71, -0.71, 0), glm::vec2(0.5, 0)),
                         Vertex(glm::vec3(-1.0f, 0.0f, 1.0f),
                                glm::vec3(-0.71, -0.71, 0), glm::vec2(1, 1)),

                         // Basso Dietro
                         Vertex(glm::vec3(-1.0f, 0.0f, -1.0f),
                                glm::vec3(0, -0.71, -0.71), glm::vec2(1, 1)),
                         Vertex(glm::vec3(1.0f, 0.0f, -1.0f),
                                glm::vec3(0, -0.71, -0.71), glm::vec2(0, 1)),
                         Vertex(glm::vec3(0.0f, -1.0f, 0.0f),
                                glm::vec3(0, -0.71, -0.71), glm::vec2(0.5, 0))};

  glGenVertexArrays(1, &(_VAO));
  glBindVertexArray(_VAO);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(
      ATTRIB_POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      reinterpret_cast<GLvoid*>(offsetof(struct Vertex, position)));
  glEnableVertexAttribArray(ATTRIB_POSITIONS);

  glVertexAttribPointer(
      ATTRIB_NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      reinterpret_cast<GLvoid*>(offsetof(struct Vertex, normal)));
  glEnableVertexAttribArray(ATTRIB_NORMALS);

  glVertexAttribPointer(
      ATTRIB_COLOR_TEXTURE_COORDS, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      reinterpret_cast<GLvoid*>(offsetof(struct Vertex, textcoord)));
  glEnableVertexAttribArray(ATTRIB_COLOR_TEXTURE_COORDS);

  _initialized = true;

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  _color_texture1.load("holo.png");
  _color_texture2.load("gold.png");
}

void Octahedron::render(void) {
  if (!_initialized) init();

  glBindVertexArray(_VAO);

  _color_texture1.bind(TEXTURE_COLOR);
  glDrawArrays(GL_TRIANGLES, 0, 12);

  _color_texture2.bind(TEXTURE_COLOR);
  glDrawArrays(GL_TRIANGLES, 12, 12);

  glBindVertexArray(0);
}
