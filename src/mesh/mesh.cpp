#include "mesh.h"

#include <iostream>
#include <unordered_map>
#include <map>
#include <unordered_set>

#include <assimp/Importer.hpp>  // Assimp Importer object

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "vertex.h"
#include "../logger.h"
#include "../utilities.h"
#include "halfedge.h"

AbstractMesh::AbstractMesh(const MESH_TYPE type, HalfEdgeData* hf_data,
                           Material* material)
    : hf_data_(hf_data), material_(material) {
  LOG_TRACE("const MESH_TYPE , HalfEdgeData* , const Material& ");
  if (!hf_data->IsValidType(type)) {
    throw;
  }
  // TODO FIX BAD CALL TO VIRTUAL FUNCTION IN CONSTRUCTOR
  GenerateOpenGLBuffers();
}

// this is weird because it is a pure virtual function but whatever
AbstractMesh::~AbstractMesh() {
  LOG_TRACE("~AbstractMesh()");

  delete hf_data_;
  ClearOpenGLBuffers();
}

const GLuint& AbstractMesh::vao() const {
  return VAO_;
}

unsigned int AbstractMesh::num_indices() const {
  return num_indices_;
}

const HalfEdgeData* AbstractMesh::half_edge_data() const {
  return hf_data_;
}

const Material* AbstractMesh::material() const {
  return material_;
}

Material* AbstractMesh::material() {
  return material_;
}

void AbstractMesh::material(Material* m) {
  if (material_ != nullptr) {
    delete material_;
  }
  material_ = m;
}

bool AbstractMesh::IsManifold() const {
  return hf_data_->IsManifold();
}

int AbstractMesh::num_vertices() const {
  return hf_data_->vertices()->size();
}

int AbstractMesh::num_edges() const {
  return hf_data_->edges()->size();
}

int AbstractMesh::num_faces() const {
  return hf_data_->faces()->size();
}

void AbstractMesh::ApplySmoothNormals() {
  hf_data_->ShadeSmooth();
}

void AbstractMesh::GenerateOpenGLBuffers(std::vector<Vertex>* vertices,
                                         std::vector<unsigned int>* indices) {
  ClearOpenGLBuffers();
  if (vertices == nullptr) {
    vertices = CreateVertexBuffer(hf_data_);
  }
  if (indices == nullptr) {
    indices = CreateIndexBuffer(hf_data_);
  }
  num_indices_ = indices->size();

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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * num_indices_,
               indices->data(), GL_STATIC_DRAW);
  glBindVertexArray(0);

  // we don't need them anymore
  delete vertices;
  delete indices;
}

void AbstractMesh::GenerateOpenGLBuffersWithSmoothShading() {
  ClearOpenGLBuffers();
  HalfEdgeData* shaded = new HalfEdgeData(*hf_data_);
  shaded->ShadeSmooth();
  std::vector<Vertex>* vertex_buffer = CreateVertexBuffer(shaded);
  std::vector<unsigned int>* index_buffer = CreateIndexBuffer(shaded);
  GenerateOpenGLBuffers(vertex_buffer, index_buffer);
  delete shaded;
}

void AbstractMesh::GenerateOpenGLBufferWithFlatShading() {
  // custom vertex and index generation for duplicating vertices
  std::vector<Vertex>* vertex_buffer = new std::vector<Vertex>();
  std::vector<unsigned int>* index_buffer = new std::vector<unsigned int>();

  for (const Face* f : *hf_data_->faces()) {
    glm::vec3 face_normal = glm::normalize(f->ComputeNormalWithArea());

    HalfEdge* start = f->halfedge;
    HalfEdge* curr = start;

    do {
      vertex_buffer->emplace_back(curr->vert->position, face_normal,
                                  curr->vert->text_coords);
      curr = curr->next;
    } while (curr != start);
  }

  int i = 0;
  for (const Face* x : *hf_data_->faces()) {
    HalfEdge* start = x->halfedge;
    HalfEdge* curr = start;

    do {
      index_buffer->push_back(i);
      i++;
      curr = curr->next;
    } while (curr != start);
  }

  GenerateOpenGLBuffers(vertex_buffer, index_buffer);
}

void AbstractMesh::ClearOpenGLBuffers() {
  glDeleteBuffers(1, &VBO_);
  glDeleteBuffers(1, &IBO_);
  glDeleteVertexArrays(1, &VAO_);
  num_indices_ = 0;
}

TriMesh::TriMesh(HalfEdgeData* hf_data, Material* material)
    : AbstractMesh(MESH_TYPE::TRI, hf_data, material) {
  //
}

int TriMesh::PatchNumVertices() const {
  return to_underlying(MESH_TYPE::TRI);
}

std::vector<sa::SubDiv> TriMesh::CompatibleSubdivs() {
  std::vector<sa::SubDiv> res = {sa::SubDiv::NONE, sa::SubDiv::LOOP};
  if (IsManifold()) {
    res.push_back(sa::SubDiv::SQRT3);
  }
  return res;
}

IMesh* TriMesh::clone() {
  return new TriMesh(new HalfEdgeData(*this->hf_data_), this->material_);
}

QuadMesh::QuadMesh(HalfEdgeData* hf_data, Material* material)
    : AbstractMesh(MESH_TYPE::QUAD, hf_data, material) {
  //
}

int QuadMesh::PatchNumVertices() const {
  return to_underlying(MESH_TYPE::QUAD);
}

std::vector<sa::SubDiv> QuadMesh::CompatibleSubdivs() {
  return {sa::SubDiv::NONE, sa::SubDiv::CATMULL};
}

IMesh* QuadMesh::clone() {
  return new QuadMesh(new HalfEdgeData(*this->hf_data_), this->material_);
}

PolyMesh::PolyMesh(HalfEdgeData* hf_data, Material* material)
    : AbstractMesh(MESH_TYPE::POLY, hf_data, material) {
  //
}

int PolyMesh::PatchNumVertices() const {
  // NOTE this mesh binds a triangulated version of itself to the gpu
  return to_underlying(MESH_TYPE::TRI);
}

std::vector<sa::SubDiv> PolyMesh::CompatibleSubdivs() {
  return {sa::SubDiv::NONE};
}

IMesh* PolyMesh::clone() {
  return new PolyMesh(new HalfEdgeData(*this->hf_data_), this->material_);
}
