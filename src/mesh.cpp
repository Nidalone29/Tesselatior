#include "mesh.h"

#include <iostream>

#include <assimp/Importer.hpp>  // Assimp Importer object

#include "vertex.h"
#include "logger.h"
#include "utilities.h"

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<unsigned int>& indices,
           const unsigned int num_indices, const Material& material)
    : vertices_(vertices),
      indices_(indices),
      num_indices_(num_indices),
      material_(material) {
  LOG_TRACE(
      "Mesh(const std::vector<Vertex>&, const std::vector<unsigned int>&, "
      "const unsigned int, const Material&)");

  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);

  // Vertex Buffer Object is NOT bound to the Vertex Array Object
  // the actual association between an attribute index and a buffer is made
  // by glVertexAttribPointer
  // https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object

  glGenBuffers(1, &VBO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_.size(),
               vertices_.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(to_underlying(ATTRIB_ID::POSITIONS), 3, GL_FLOAT,
                        GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(struct Vertex, position));
  glVertexAttribPointer(to_underlying(ATTRIB_ID::NORMALS), 3, GL_FLOAT,
                        GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(struct Vertex, normal));

  // we are talking about the texture color here
  glVertexAttribPointer(to_underlying(ATTRIB_ID::COLOR_TEXTURE_COORDS), 2,
                        GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(struct Vertex, text_coords));

  glGenBuffers(1, &IBO_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices_.size(),
               indices_.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
}

Mesh::~Mesh() {
  LOG_TRACE("~Mesh()");
}

const GLuint& Mesh::vao() const {
  return VAO_;
}

const unsigned int& Mesh::num_indices() const {
  return num_indices_;
}

const Material& Mesh::material() const {
  return material_;
}

void Mesh::material(const Material& material) {
  material_ = material;
}
