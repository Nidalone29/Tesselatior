#include "mesh.h"

#include <assimp/Importer.hpp>  // Assimp Importer object
#include <iostream>

#include "vertex.h"

// TODO finish mesh importer
// figure out how to deal with textures, because the mess of the filepath is
// caused by that
Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<unsigned int>& indices,
           const unsigned int num_indices, const Material& material)
    : _vertices(vertices),
      _indices(indices),
      _num_indices(num_indices),
      _material(material),
      _loaded(false) {
  std::cout << "mesh created" << std::endl;
}

void Mesh::load() {
  // impostiamo il contex OpenGL
  glGenVertexArrays(1, &_VAO);
  glBindVertexArray(_VAO);

  glGenBuffers(1, &_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _vertices.size(),
               &_vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &_IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _indices.size(),
               &_indices[0], GL_STATIC_DRAW);

  glVertexAttribPointer(ATTRIB_POSITIONS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(struct Vertex, _position));
  glVertexAttribPointer(ATTRIB_NORMALS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(struct Vertex, _normal));

  // we are talking about the texture color here
  glVertexAttribPointer(ATTRIB_COLOR_TEXTURE_COORDS, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex),
                        (void*)offsetof(struct Vertex, _text_coords));

  glBindVertexArray(0);
  _loaded = true;
}

const GLuint& Mesh::getVAO() const {
  return _VAO;
}

const GLuint& Mesh::getVBO() const {
  return _VBO;
}

const GLuint& Mesh::getIBO() const {
  return _IBO;
}

const unsigned int& Mesh::get_num_indices() const {
  return _num_indices;
}

const Material& Mesh::getMaterial() const {
  return _material;
}

const bool Mesh::isLoaded() const {
  return _loaded;
}
