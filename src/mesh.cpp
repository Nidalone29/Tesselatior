#include "mesh.h"

#include <unordered_map>
#include <map>

#include <assimp/Importer.hpp>  // Assimp Importer object

#include "vertex.h"
#include "logger.h"
#include "utilities.h"

Mesh::Mesh(const MESH_TYPE type, std::vector<Vertex*>* vertices,
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
  LOG_ERROR("VERTICES SIZE {}", vertices_->size());
  LOG_ERROR("HF SIZE {}", half_edges_->size());
  LOG_ERROR("FACES SIZE {}", faces_->size());
  LOG_ERROR("EDGES SIZE {}", edges_->size());
  GenerateOpenGLBuffers();
}

Mesh::Mesh(const Mesh& other) : material_(other.material_), type_(other.type_) {
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
    vertices_->push_back(other.vertices_->at(i));
    vertices_->at(i)->halfedge = nullptr;
  }

  using Uint = unsigned int;
  std::vector<Uint>* indices_temp = CreateIndexBuffer();

  std::map<std::pair<Uint, Uint>, HalfEdge*> edges_m;

  std::pair<Uint, Uint> x;
  std::pair<Uint, Uint> s;  // swapped

  for (int i = 0; i < indices_temp->size(); i += to_underlying(type_)) {
    HalfEdge* current_hf;
    Face* f = new Face();
    faces_->push_back(f);
    std::vector<HalfEdge*> faces_halfedges;
    for (int k = 0; k < to_underlying(type_); k++) {
      x = {indices_temp->at(i + k),
           indices_temp->at((i + k + 1) % to_underlying(type_))};
      s = {x.second, x.first};

      current_hf = new HalfEdge(f);
      edges_m[x] = current_hf;
      f->halfedge = current_hf;
      current_hf->vert = vertices_->at(x.second);
      vertices_->at(x.first)->halfedge = current_hf;

      if (edges_m.find(s) != edges_m.end()) {
        edges_m[x]->twin = edges_m[s];
        edges_m[s]->twin = edges_m[x];
      }

      half_edges_->push_back(current_hf);
      current_hf = current_hf->next;
    }

    for (int z = 0; z < faces_halfedges.size(); z++) {
      faces_halfedges[z]->next =
          faces_halfedges[(z + 1) % faces_halfedges.size()];
    }
  }

  for (const auto [_, he] : edges_m) {
    Edge* e = new Edge();
    e->halfedge = he;
    edges_->push_back(e);
  }

  delete indices_temp;

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
    std::swap(this->edges_, temp.edges_);
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

  for (Edge* x : *edges_) {
    delete x;
  }
  delete edges_;

  for (HalfEdge* x : *half_edges_) {
    delete x;
  }
  delete half_edges_;

  for (Vertex* x : *vertices_) {
    delete x;
  }
  delete vertices_;

  // delete indices_;
  ClearOpenGLBuffers();
}

void Mesh::GenerateOpenGLBuffers() {
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

void Mesh::ClearOpenGLBuffers() {
  glDeleteBuffers(1, &VBO_);
  glDeleteBuffers(1, &IBO_);
  glDeleteVertexArrays(1, &VAO_);
}

const GLuint& Mesh::vao() const {
  return VAO_;
}

unsigned int Mesh::num_indices() const {
  return faces_->size() * to_underlying(type_);
}

unsigned int Mesh::num_vertices() const {
  return vertices_->size();
}

std::vector<Vertex*>* Mesh::vertices() {
  return vertices_;
}

std::vector<HalfEdge*>* Mesh::half_edges() {
  return half_edges_;
}

std::vector<Face*>* Mesh::faces() {
  return faces_;
}

std::vector<Edge*>* Mesh::edges() {
  return edges_;
}

const Material& Mesh::material() const {
  return material_;
}

void Mesh::material(const Material& material) {
  material_ = material;
}

void Mesh::split(Edge* e) {
  if (type_ == MESH_TYPE::QUADS) {
    // TODO REFACTOR
    throw;
  }

  Face* f0 = e->halfedge->face;
  HalfEdge* h0 = e->halfedge;
  HalfEdge* h1 = e->halfedge->next;
  HalfEdge* h2 = e->halfedge->next->next;

  Face* f1 = e->halfedge->twin->face;
  HalfEdge* h3 = e->halfedge->twin;
  HalfEdge* h4 = e->halfedge->twin->next;
  HalfEdge* h5 = e->halfedge->twin->next->next;

  Vertex* v0 = e->halfedge->vert;
  Vertex* v1 = e->halfedge->next->vert;
  Vertex* v2 = e->halfedge->twin->vert;
  Vertex* v3 = e->halfedge->twin->next->vert;

  // 2 new faces
  Face* f2 = new Face();
  Face* f3 = new Face();

  // 6 new halfedges
  HalfEdge* h6 = new HalfEdge(f0);
  HalfEdge* h7 = new HalfEdge(f2);
  HalfEdge* h8 = new HalfEdge(f2);
  HalfEdge* h9 = new HalfEdge(f1);
  HalfEdge* h10 = new HalfEdge(f3);
  HalfEdge* h11 = new HalfEdge(f3);
  // update old halfedges faces
  h2->face = f2;
  h4->face = f3;
  // update old halfedges successors
  h1->next = h6;
  h2->next = h8;
  h3->next = h9;
  h4->next = h10;
  // update new halfedges successors
  h6->next = h0;
  h7->next = h2;
  h8->next = h7;
  h11->next = h4;
  h10->next = h11;
  h9->next = h5;
  // update new halfedges twins
  h6->twin = h7;
  h7->twin = h6;
  h8->twin = h11;
  h11->twin = h8;
  h9->twin = h10;
  h10->twin = h9;

  // 3 new edges
  Edge* e1 = new Edge();
  e1->halfedge = h7;
  Edge* e2 = new Edge();
  e2->halfedge = h8;
  Edge* e3 = new Edge();
  e3->halfedge = h10;

  // 1 new vertex
  // for now only position, TODO check uv and normal

  glm::vec3 new_pos = glm::mix(v2->position, v0->position, 0.5F);
  glm::vec3 new_norm = glm::mix(v2->normal, v0->normal, 0.5F);
  glm::vec2 new_uv = glm::mix(v2->text_coords, v0->text_coords, 0.5F);
  Vertex* v4 = new Vertex(new_pos, new_norm, new_uv);

  // this might be a relocation... invalidating every pointer to vert... BRUH
  // what do I do???
  // ffs...
  // AAAAAAAAAAAAAAAAAAAAAAAH
  // vertices_->emplace_back(new_pos, new_norm, new_uv);
}

// you have the responsibility to delete the vector
std::vector<unsigned int>* Mesh::CreateIndexBuffer() const {
  std::vector<unsigned int>* index_buffer = new std::vector<unsigned int>();
  index_buffer->reserve(faces_->size() * to_underlying(type_));

  std::map<Vertex*, unsigned int> added_verts;

  int i = 0;
  for (Face* x : *faces_) {
    HalfEdge* curr = x->halfedge;
    for (int j = 0; j < to_underlying(type_); j++) {
      if (added_verts.find(curr->vert) == added_verts.end()) {
        added_verts[curr->vert] = i;
        i++;
      }
      index_buffer->push_back(added_verts[curr->vert]);
      curr = curr->next;
    }
  }

  LOG_ERROR(index_buffer->size());
  return index_buffer;
}

// you have the responsibility to delete the vector
std::vector<Vertex>* Mesh::CreateVertexBuffer() const {
  std::vector<Vertex>* vert_buffer = new std::vector<Vertex>();
  vert_buffer->reserve(vertices_->size());

  for (Vertex* x : *vertices_) {
    vert_buffer->push_back(*x);
  }

  return vert_buffer;
}
