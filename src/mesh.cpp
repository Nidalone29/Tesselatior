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
  }

  return *this;
}

Mesh::~Mesh() {
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

void Mesh::GenerateOpenGLBuffers() {
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

MESH_TYPE Mesh::type() const {
  return type_;
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

void Mesh::split(Edge* e, const Vertex& new_vert) {
  if (type_ == MESH_TYPE::QUADS) {
    // TODO REFACTOR
    throw;
  }

  if (e->halfedge->IsBoundary()) {
    Face* f0 = e->halfedge->face;
    HalfEdge* h0 = e->halfedge;  // this is the boundary 100%
    HalfEdge* h1 = e->halfedge->next;
    HalfEdge* h2 = e->halfedge->next->next;

    Vertex* v0 = h0->vert;
    Vertex* v1 = h1->vert;
    Vertex* v2 = h2->vert;

    // 1 new face
    Face* f1 = new Face();

    // 3 new halfedges
    HalfEdge* h3 = new HalfEdge(f0);
    HalfEdge* h4 = new HalfEdge(f1);
    HalfEdge* h5 = new HalfEdge(f1);

    // update halfedges faces
    h1->face = f1;
    // update halfedges successors
    h1->next = h4;
    h4->next = h5;
    h5->next = h1;

    h0->next = h3;
    h3->next = h2;
    h2->next = h0;

    // update hafledges twins
    h3->twin = h4;
    h4->twin = h3;

    // new edge
    // 2 new edges
    Edge* e3 = new Edge();
    e3->halfedge = h5;
    Edge* e4 = new Edge();
    e4->halfedge = h4;

    // set halfedges edges
    h4->edge = e4;
    h5->edge = e3;

    h3->edge = e4;

    // 1 new vertex
    // Let's just try like this
    Vertex* v3 =
        new Vertex(new_vert.position, new_vert.normal, new_vert.text_coords);
    vertices_->push_back(v3);
    v3->halfedge = h3;

    f1->halfedge = h1;
    f0->halfedge = h0;

    h4->vert = v3;
    h5->vert = v0;
    h1->vert = v1;

    h3->vert = v1;
    h2->vert = v2;
    h0->vert = v3;

    faces_->push_back(f1);
    half_edges_->push_back(h3);
    half_edges_->push_back(h4);
    half_edges_->push_back(h5);
    edges_->push_back(e3);
    edges_->push_back(e4);
  } else {  // [[likely]]
    Face* f0 = e->halfedge->face;
    HalfEdge* h0 = e->halfedge;
    HalfEdge* h1 = e->halfedge->next;
    HalfEdge* h2 = e->halfedge->next->next;

    Face* f1 = e->halfedge->twin->face;
    HalfEdge* h3 = e->halfedge->twin;
    HalfEdge* h4 = e->halfedge->twin->next;
    HalfEdge* h5 = e->halfedge->twin->next->next;

    Vertex* v0 = h3->vert;
    Vertex* v1 = h1->vert;
    Vertex* v2 = h0->vert;
    Vertex* v3 = h4->vert;
    assert(v0 == h2->vert);
    assert(v2 == h5->vert);

    HalfEdge* t1 = h2->twin;
    HalfEdge* t2 = h1->twin;
    HalfEdge* t3 = h5->twin;
    HalfEdge* t4 = h4->twin;

    Edge* e1 = h2->edge;
    Edge* e2 = h1->edge;
    Edge* e3 = h5->edge;
    Edge* e4 = h4->edge;
    assert(e1 == t1->edge);
    assert(e2 == t2->edge);
    assert(e3 == t3->edge);
    assert(e4 == t4->edge);

    // now building the splitted face
    // 2 new faces
    Face* f2 = new Face();
    Face* f3 = new Face();

    // 6 new halfedges
    h0->face = f0;
    h1->face = f2;
    h2->face = f0;
    h3->face = f1;
    h4->face = f1;
    h5->face = f3;
    HalfEdge* h6 = new HalfEdge(f0);
    HalfEdge* h7 = new HalfEdge(f2);
    HalfEdge* h8 = new HalfEdge(f2);
    HalfEdge* h9 = new HalfEdge(f3);
    HalfEdge* h10 = new HalfEdge(f3);
    HalfEdge* h11 = new HalfEdge(f1);

    // 3 new edges
    Edge* e5 = new Edge();
    e5->halfedge = h6;
    Edge* e6 = new Edge();
    e6->halfedge = h8;
    Edge* e7 = new Edge();
    e7->halfedge = h10;

    // successors
    // f0
    h0->next = h6;
    h6->next = h2;
    h2->next = h0;
    // f1
    h3->next = h4;
    h4->next = h11;
    h11->next = h3;
    //  f2
    h7->next = h8;
    h8->next = h1;
    h1->next = h7;
    // f3
    h9->next = h10;
    h10->next = h5;
    h5->next = h9;

    // twins
    // e
    h0->twin = h3;
    h3->twin = h0;
    // e1
    h2->twin = t1;
    t1->twin = h2;
    // e2
    h1->twin = t2;
    t2->twin = h1;
    // e3
    h5->twin = t3;
    t3->twin = h5;
    // e4
    h4->twin = t4;
    t4->twin = h4;
    // e5
    h6->twin = h7;
    h7->twin = h6;
    // e6
    h8->twin = h9;
    h9->twin = h8;
    // e7
    h10->twin = h11;
    h11->twin = h10;

    // update faces
    f0->halfedge = h0;
    f1->halfedge = h4;
    f2->halfedge = h7;
    f3->halfedge = h9;

    // set halfedges edges
    h0->edge = e;
    h1->edge = e2;
    h2->edge = e1;
    h3->edge = e;
    h4->edge = e4;
    h5->edge = e3;
    h6->edge = e5;
    h7->edge = e5;
    h8->edge = e6;
    h9->edge = e6;
    h10->edge = e7;
    h11->edge = e7;

    Vertex* v4 =
        new Vertex(new_vert.position, new_vert.normal, new_vert.text_coords);
    v4->halfedge = h10;
    v0->halfedge = h0;
    v1->halfedge = h7;
    v2->halfedge = h9;
    v3->halfedge = h5;

    // vertices
    // f0
    h0->vert = v4;
    h6->vert = v1;
    h2->vert = v0;
    // f1
    h3->vert = v0;
    h4->vert = v3;
    h11->vert = v4;
    //  f2
    h7->vert = v4;
    h8->vert = v2;
    h1->vert = v1;
    // f3
    h9->vert = v4;
    h10->vert = v3;
    h5->vert = v2;
    assert(t1->vert == v1);
    assert(t2->vert == v2);
    assert(t3->vert == v3);
    assert(t4->vert == v0);

    vertices_->push_back(v4);
    faces_->push_back(f2);
    faces_->push_back(f3);
    half_edges_->push_back(h6);
    half_edges_->push_back(h7);
    half_edges_->push_back(h8);
    half_edges_->push_back(h9);
    half_edges_->push_back(h10);
    half_edges_->push_back(h11);

    // this edge is the other half of the original edge
    edges_->push_back(e6);

    // and these ones are new
    edges_->push_back(e5);
    edges_->push_back(e7);
  }
}

void Mesh::flip(const Edge* e) {
  if (e->halfedge->IsBoundary()) {
    LOG_ERROR("YOU CAN'T FLIP A BOUNDARY EDGE");
    throw;
  }

  // f0
  HalfEdge* h0 = e->halfedge;
  HalfEdge* h1 = e->halfedge->next;
  HalfEdge* h2 = e->halfedge->next->next;

  // f1
  HalfEdge* h3 = e->halfedge->twin;
  HalfEdge* h4 = e->halfedge->twin->next;
  HalfEdge* h5 = e->halfedge->twin->next->next;

  Face* f0 = e->halfedge->face;
  Face* f1 = e->halfedge->twin->face;

  Vertex* v0 = h0->vert;
  Vertex* v1 = h1->vert;
  Vertex* v2 = h2->vert;
  Vertex* v3 = h4->vert;

  // twins
  HalfEdge* t1 = h1->twin;
  HalfEdge* t2 = h2->twin;
  HalfEdge* t3 = h4->twin;
  HalfEdge* t4 = h5->twin;

  // f0
  h0->next = h5;
  h5->next = h1;
  h1->next = h0;

  // f1
  h3->next = h2;
  h2->next = h4;
  h4->next = h3;

  // vertices
  h0->vert = v3;
  h3->vert = v1;

  h1->vert = v1;
  h5->vert = v0;

  h2->vert = v2;
  h4->vert = v3;

  // faces
  // f0
  h0->face = f0;
  h5->face = f0;
  h1->face = f0;

  // f1
  h3->face = f1;
  h2->face = f1;
  h4->face = f1;

  f0->halfedge = h0;
  f1->halfedge = h3;

  v0->halfedge = h1;
  v1->halfedge = h2;
  v2->halfedge = h4;
  v3->halfedge = h5;

  h4->twin = t3;
  h5->twin = t4;
  h1->twin = t1;
  h2->twin = t2;
}

// you have the responsibility to delete the vector
std::vector<unsigned int>* Mesh::CreateIndexBuffer() const {
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
std::vector<Vertex>* Mesh::CreateVertexBuffer() const {
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
