#include "mesh.h"

#include <iostream>

#include <assimp/Importer.hpp>  // Assimp Importer object

#include "vertex.h"
#include "logger.h"
#include "utilities.h"

// TODO finish mesh importer
// figure out how to deal with textures, because the mess of the filepath is
// caused by that
Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<unsigned int>& indices,
           const unsigned int num_indices, const Material& material)
    : _vertices(vertices),
      _indices(indices),
      _num_indices(num_indices),
      _material(material) {
  LOG_TRACE(
      "Mesh(const std::vector<Vertex>&, const std::vector<unsigned int>&, "
      "const unsigned int, const Material&)");

  glGenVertexArrays(1, &_VAO);
  glBindVertexArray(_VAO);
  // Vertex Buffer Object is NOT bound to the Vertex Array Object
  // the actual association between an attribute index and a buffer is made
  // by glVertexAttribPointer
  // https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
  glGenBuffers(1, &_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _vertices.size(),
               _vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(to_underlying(ATTRIB_ID::POSITIONS), 3, GL_FLOAT,
                        GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(struct Vertex, _position));
  glVertexAttribPointer(to_underlying(ATTRIB_ID::NORMALS), 3, GL_FLOAT,
                        GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(struct Vertex, _normal));

  // we are talking about the texture color here
  glVertexAttribPointer(to_underlying(ATTRIB_ID::COLOR_TEXTURE_COORDS), 2,
                        GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(struct Vertex, _text_coords));

  glGenBuffers(1, &_IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _indices.size(),
               _indices.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
}

Mesh::~Mesh() {
  LOG_TRACE("~Mesh()");
}

const GLuint& Mesh::getVAO() const {
  return _VAO;
}

const unsigned int& Mesh::get_num_indices() const {
  return _num_indices;
}

const Material& Mesh::getMaterial() const {
  return _material;
}
