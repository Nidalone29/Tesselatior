#include "mesh.h"

#include <iostream>
#include <map>

#include <assimp/Importer.hpp>  // Assimp Importer object

#include "vertex.h"
#include "logger.h"
#include "utilities.h"

Mesh::Mesh(std::vector<Vertex>* vertices, std::vector<HalfEdge*>* halfedges,
           std::vector<Face*>* faces, std::vector<unsigned int> indices,
           const Material& material)
    : VAO_(-1),
      VBO_(-1),
      IBO_(-1),
      vertices_(vertices),
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
  GenerateOpenGLBuffers();
}

Mesh::Mesh(const Mesh& other)
    : VAO_(-1),
      VBO_(-1),
      IBO_(-1),
      indices_(other.indices_),
      material_(other.material_),
      type_(other.type_) {
  LOG_TRACE("Mesh(const Mesh& other)");
  vertices_ = new std::vector<Vertex>();
  half_edges_ = new std::vector<HalfEdge*>();
  faces_ = new std::vector<Face*>();
  vertices_->reserve(other.vertices_->size());
  half_edges_->reserve(other.half_edges_->size());
  faces_->reserve(other.faces_->size());

  for (int i = 0; i < other.vertices_->size(); i++) {
    vertices_->push_back(other.vertices_->at(i));
    vertices_->at(i).edge = nullptr;
  }

  using Uint = unsigned int;
  std::map<std::pair<Uint, Uint>, HalfEdge*> edges_m;

  std::pair<Uint, Uint> x;
  std::pair<Uint, Uint> s;  // swapped

  for (int i = 0; i < indices_.size(); i += to_underlying(type_)) {
    HalfEdge* current_hf;
    Face* f = new Face();
    faces_->push_back(f);
    for (int k = 0; k < to_underlying(type_); k++) {
      x = {indices_[i + k], indices_[(i + k + 1) % to_underlying(type_)]};
      s = {x.second, x.first};

      current_hf = new HalfEdge(f);
      edges_m[x] = current_hf;
      f->halfedge = current_hf;
      current_hf->vert = &vertices_->at(x.second);
      vertices_->at(x.first).edge = current_hf;

      if (edges_m.find(s) != edges_m.end()) {
        edges_m[x]->twin = edges_m[s];
        edges_m[s]->twin = edges_m[x];
      }

      half_edges_->push_back(current_hf);
      current_hf = current_hf->next;
    }
  }

  GenerateOpenGLBuffers();
}

Mesh& Mesh::operator=(const Mesh& other) {
  LOG_TRACE("Mesh& operator=(const Mesh& other)");
  if (this != &other) {
    Mesh temp(other);
    std::swap(this->IBO_, temp.IBO_);
    std::swap(this->VAO_, temp.VAO_);
    std::swap(this->VBO_, temp.VBO_);
    std::swap(this->type_, temp.type_);
    std::swap(this->vertices_, temp.vertices_);
    std::swap(this->half_edges_, temp.half_edges_);
    std::swap(this->faces_, temp.faces_);
    std::swap(this->material_, temp.material_);
    // here temp will call its destructor, but with the empty variables it
    // should be no problem
  }

  return *this;
}

Mesh::~Mesh() {
  LOG_TRACE("~Mesh()");

  for (Face* x : *faces_) {
    delete x;
  }
  delete faces_;

  delete vertices_;

  for (HalfEdge* x : *half_edges_) {
    delete x;
  }
  delete half_edges_;

  // delete indices_;
  ClearOpenGLBuffers();
}

void Mesh::GenerateOpenGLBuffers() {
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

void Mesh::ClearOpenGLBuffers() {
  glDeleteBuffers(1, &VBO_);
  VBO_ = -1;
  glDeleteBuffers(1, &IBO_);
  IBO_ = -1;
  glDeleteVertexArrays(1, &VAO_);
  VAO_ = -1;
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
