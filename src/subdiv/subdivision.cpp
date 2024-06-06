#include "subdivision.h"

#include <unordered_set>

#include "../logger.h"

ISubdivision::~ISubdivision() {
  //
}

// loosely inspired by https://github.com/cmu462/Scotty3D/wiki/Loop-Subdivision
Mesh* LoopSubdiv::subdivide(const Model* in, int n_steps) {
  Mesh* subdivided = new Mesh(in->meshes()[0]);

  for (int i = 0; i < n_steps; i++) {
    LOG_ERROR("loop subdiv {}", i + 1);
    const int edges_size = subdivided->edges()->size();
    // splitting the edges
    std::unordered_set<Vertex*> new_vertices;
    std::unordered_set<Edge*> new_edges;

    for (int j = 0; j < edges_size; j++) {
      subdivided->split(subdivided->edges()->at(j));

      Vertex* x = subdivided->vertices()->back();
      new_vertices.insert(x);
      if (subdivided->edges()->at(j)->halfedge->IsBoundary()) {
        new_edges.insert(subdivided->edges()->back());
      } else {
        new_edges.insert(subdivided->edges()->back());
        new_edges.insert(
            subdivided->edges()->at(subdivided->edges()->size() - 2));
      }
    }

    // flipping the edges
    for (Edge* e : new_edges) {
      if (!e->halfedge->IsBoundary()) {
        if ((new_vertices.find(e->halfedge->vert) != new_vertices.end() &&
             new_vertices.find(e->halfedge->twin->vert) ==
                 new_vertices.end()) ||
            (new_vertices.find(e->halfedge->vert) == new_vertices.end() &&
             new_vertices.find(e->halfedge->twin->vert) !=
                 new_vertices.end())) {
          subdivided->flip(e);
        }
      }
    }
  }

  subdivided->GenerateOpenGLBuffers();
  return subdivided;
}
