#include "subdivision.h"

#include "../logger.h"

ISubdivision::~ISubdivision() {
  //
}

Mesh* LoopSubdiv::subdivide(const Model* in, int n_steps) {
  Mesh* subdivided = new Mesh(in->meshes()[0]);

  std::vector<Vertex>* new_vertices = new std::vector<Vertex>();
  std::vector<HalfEdge*>* new_halfedges = new std::vector<HalfEdge*>();
  std::vector<Face*>* new_faces = new std::vector<Face*>();
  std::vector<unsigned int> new_indices;

  // for (Edge* e : *subdivided->edges()) {
  // subdivided->split(e);
  // e->split();
  //}

  return subdivided;
}
