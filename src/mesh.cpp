#include "mesh.h"

#include <iostream>
#include <unordered_map>
#include <map>
#include <unordered_set>

#include <assimp/Importer.hpp>  // Assimp Importer object

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "vertex.h"
#include "logger.h"
#include "utilities.h"

Mesh::~Mesh() {
  //
}

TriMesh::TriMesh(const MESH_TYPE type, std::vector<Vertex*>* vertices,
                 std::vector<HalfEdge*>* halfedges, std::vector<Face*>* faces,
                 std::vector<Edge*>* edges, const Material& material)
    : vertices_(vertices),
      half_edges_(halfedges),
      faces_(faces),
      edges_(edges),
      type_(type),
      material_(material) {
  LOG_TRACE(
      "Mesh(std::vector<Vertex>*, std::vector<HalfEdge*>*, "
      "std::vector<Face*>*, std::vector<Edge*>*, std::vector<unsigned int>*, "
      "const Material&)");
  GenerateOpenGLBuffers();
}

TriMesh::TriMesh(const TriMesh& other)
    : material_(other.material_), type_(other.type_) {
  LOG_TRACE("Mesh(const Mesh& other)");

  vertices_ = new std::vector<Vertex*>();
  half_edges_ = new std::vector<HalfEdge*>();
  faces_ = new std::vector<Face*>();
  edges_ = new std::vector<Edge*>();

  vertices_->reserve(other.vertices_->size());
  half_edges_->reserve(other.half_edges_->size());
  faces_->reserve(other.faces_->size());
  edges_->reserve(other.edges_->size());

  for (int i = 0; i < other.vertices_->size(); i++) {
    Vertex* x = new Vertex(other.vertices_->at(i)->position,
                           other.vertices_->at(i)->normal,
                           other.vertices_->at(i)->text_coords);
    vertices_->push_back(x);
  }

  using Uint = unsigned int;
  std::vector<Uint>* indices_temp = other.CreateIndexBuffer();

  std::map<std::pair<Uint, Uint>, HalfEdge*> edges_m;

  for (int i = 0; i < other.faces_->size(); i++) {
    HalfEdge* current_hf;
    Face* f = new Face();
    faces_->push_back(f);

    std::pair<Uint, Uint> x;
    std::pair<Uint, Uint> s;  // swapped

    std::vector<HalfEdge*> faces_halfedges;

    for (int k = 0; k < to_underlying(type_); k++) {
      x = {indices_temp->at(to_underlying(type_) * i + k),
           indices_temp->at(to_underlying(type_) * i +
                            ((k + 1) % to_underlying(type_)))};
      s = {x.second, x.first};

      current_hf = new HalfEdge(f);
      faces_halfedges.push_back(current_hf);
      f->halfedge = current_hf;

      current_hf->vert = vertices_->at(x.second);
      // LOG_ERROR(x.second);
      vertices_->at(x.first)->halfedge = current_hf;

      if (edges_m.find(s) != edges_m.end()) {
        current_hf->twin = edges_m[s];
        edges_m[s]->twin = current_hf;
      } else {
        edges_m[x] = current_hf;
      }

      half_edges_->push_back(current_hf);
    }

    for (int z = 0; z < faces_halfedges.size(); z++) {
      faces_halfedges[z]->next =
          faces_halfedges[(z + 1) % faces_halfedges.size()];
    }
  }

  for (const auto [_, he] : edges_m) {
    Edge* e = new Edge();
    e->halfedge = he;
    he->edge = e;
    if (!he->IsBoundary()) {
      he->twin->edge = e;
    }
    edges_->push_back(e);
  }

  delete indices_temp;

  GenerateOpenGLBuffers();
}

TriMesh& TriMesh::operator=(const TriMesh& other) {
  LOG_TRACE("Mesh& operator=(const Mesh& other)");
  if (this != &other) {
    TriMesh temp(other);
    std::swap(this->IBO_, temp.IBO_);
    std::swap(this->VAO_, temp.VAO_);
    std::swap(this->VBO_, temp.VBO_);
    std::swap(this->type_, temp.type_);
    std::swap(this->vertices_, temp.vertices_);
    std::swap(this->half_edges_, temp.half_edges_);
    std::swap(this->faces_, temp.faces_);
    std::swap(this->edges_, temp.edges_);
    std::swap(this->material_, temp.material_);
  }

  return *this;
}

TriMesh::~TriMesh() {
  LOG_TRACE("~Mesh()");

  for (Vertex* x : *vertices_) {
    delete x;
  }
  delete vertices_;

  for (Face* x : *faces_) {
    delete x;
  }
  delete faces_;

  for (Edge* x : *edges_) {
    delete x;
  }
  delete edges_;

  for (HalfEdge* x : *half_edges_) {
    delete x;
  }
  delete half_edges_;

  // delete indices_;
  ClearOpenGLBuffers();
}

void TriMesh::GenerateOpenGLBuffers() {
  ClearOpenGLBuffers();
  std::vector<Vertex>* vertices = CreateVertexBuffer();
  std::vector<unsigned int>* indices = CreateIndexBuffer();

  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);

  // Vertex Buffer Object is NOT bound to the Vertex Array Object
  // the actual association between an attribute index and a buffer is made
  // by glVertexAttribPointer
  // https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object

  glGenBuffers(1, &VBO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices->size(),
               vertices->data(), GL_STATIC_DRAW);

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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices->size(),
               indices->data(), GL_STATIC_DRAW);
  glBindVertexArray(0);

  // we don't need them anymore
  delete vertices;
  delete indices;
}

void TriMesh::ClearOpenGLBuffers() {
  glDeleteBuffers(1, &VBO_);
  glDeleteBuffers(1, &IBO_);
  glDeleteVertexArrays(1, &VAO_);
}

const GLuint& TriMesh::vao() const {
  return VAO_;
}

unsigned int TriMesh::num_indices() const {
  return faces_->size() * to_underlying(type_);
}

unsigned int TriMesh::num_vertices() const {
  return vertices_->size();
}

MESH_TYPE TriMesh::type() const {
  return type_;
}

std::vector<Vertex*>* TriMesh::vertices() {
  return vertices_;
}

std::vector<HalfEdge*>* TriMesh::half_edges() {
  return half_edges_;
}

std::vector<Face*>* TriMesh::faces() {
  return faces_;
}

std::vector<Edge*>* TriMesh::edges() {
  return edges_;
}

const Material& TriMesh::material() const {
  return material_;
}

void TriMesh::material(const Material& material) {
  material_ = material;
}

void TriMesh::AddVertex(Vertex* v) {
  vertices_->push_back(v);
}

void TriMesh::AddHalfedge(HalfEdge* he) {
  half_edges_->push_back(he);
}

void TriMesh::AddFace(Face* f) {
  faces_->push_back(f);
}

void TriMesh::AddEdge(Edge* e) {
  edges_->push_back(e);
}

// you have the responsibility to delete the vector
std::vector<unsigned int>* TriMesh::CreateIndexBuffer() const {
  std::vector<unsigned int>* index_buffer = new std::vector<unsigned int>();
  index_buffer->reserve(faces_->size() * to_underlying(type_));

  std::unordered_map<Vertex*, unsigned int> added_verts;

  int i = 0;
  for (Face* x : *faces_) {
    HalfEdge* curr = x->halfedge;

    for (int j = 0; j < to_underlying(type_); j++) {
      // vertex not found
      if (added_verts.find(curr->vert) == added_verts.end()) {
        added_verts[curr->vert] = i;
        i++;
      }
      index_buffer->push_back(added_verts[curr->vert]);
      curr = curr->next;
    }
  }

  return index_buffer;
}

// you have the responsibility to delete the vector
std::vector<Vertex>* TriMesh::CreateVertexBuffer() const {
  std::vector<Vertex>* vertex_buffer = new std::vector<Vertex>();
  vertex_buffer->reserve(vertices_->size());

  std::unordered_set<Vertex*> added_verts;

  int i = 0;
  for (Face* x : *faces_) {
    HalfEdge* curr = x->halfedge;

    for (int j = 0; j < to_underlying(type_); j++) {
      // vertex not found
      if (added_verts.find(curr->vert) == added_verts.end()) {
        added_verts.insert(curr->vert);
        vertex_buffer->push_back(*curr->vert);
      }
      curr = curr->next;
    }
  }

  return vertex_buffer;
}
