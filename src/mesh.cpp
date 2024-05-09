#include "mesh.h"

#include <iostream>

#include <assimp/Importer.hpp>  // Assimp Importer object

#include "vertex.h"
#include "logger.h"
#include "utilities.h"

Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<HalfEdge*>* halfedges,
           std::vector<Face*>* faces, std::vector<unsigned int> indices,
           const Material& material)
    : vertices_(vertices),
      half_edges_(halfedges),
      faces_(faces),
      indices_(indices),
      material_(material) {
  LOG_ERROR("VERTICES SIZE {}", vertices_->size());
  LOG_ERROR("HF SIZE {}", half_edges_->size());
  LOG_ERROR("FACES SIZE {}", faces_->size());
  LOG_ERROR("INDEX SIZE {}", indices_.size());
  LOG_TRACE(
      "Mesh(std::vector<Vertex>*, std::vector<HalfEdge*>*, "
      "std::vector<Face*>*, std::vector<unsigned int>*, const Material&)");

  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);

  // Vertex Buffer Object is NOT bound to the Vertex Array Object
  // the actual association between an attribute index and a buffer is made
  // by glVertexAttribPointer
  // https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object

  glGenBuffers(1, &VBO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_->size(),
               vertices_->data(), GL_STATIC_DRAW);

  glVertexAttribPointer(to_underlying(ATTRIB_ID::POSITIONS), 3, GL_FLOAT,
                        GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(struct Vertex, position));
  glVertexAttribPointer(to_underlying(ATTRIB_ID::NORMALS), 3, GL_FLOAT,
                        GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(struct Vertex, normal));
  glVertexAttribPointer(to_underlying(ATTRIB_ID::TEXTURE_COORDS), 2, GL_FLOAT,
                        GL_FALSE, sizeof(Vertex),
                        (GLvoid*)offsetof(struct Vertex, text_coords));

  glGenBuffers(1, &IBO_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices_.size(),
               indices_.data(), GL_STATIC_DRAW);

  glBindVertexArray(0);
}

Mesh::~Mesh() {
  LOG_TRACE("~Mesh()");

  for (int i = 0; i < half_edges_->size(); i++) {
    delete half_edges_->at(i);
  }
  for (int i = 0; i < faces_->size(); i++) {
    delete faces_->at(i);
  }
  delete vertices_;
  // delete indices_;
}

const GLuint& Mesh::vao() const {
  return VAO_;
}

unsigned int Mesh::num_indices() const {
  return indices_.size();
}

unsigned int Mesh::num_vertices() const {
  return vertices_->size();
}

const Material& Mesh::material() const {
  return material_;
}

void Mesh::material(const Material& material) {
  material_ = material;
}
