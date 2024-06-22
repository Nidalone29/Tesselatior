#include "subdivision.h"

#include <unordered_set>

#include "../logger.h"

ISubdivision::~ISubdivision() {
  //
}

// loosely inspired by https://github.com/cmu462/Scotty3D/wiki/Loop-Subdivision
// and chapter 4.2 of
// https://graphics.stanford.edu/courses/cs348a-09-fall/Papers/zorin-subdivision00.pdf
Mesh* LoopSubdiv::subdivide(const Model* in, int n_steps) {
  Mesh* subdivided = new Mesh(in->meshes()[0]);

  for (int i = 0; i < n_steps; i++) {
    LOG_INFO("loop subdiv {}", i + 1);

    // computing the new vertex positions for the even vertices
    std::unordered_map<Vertex*, Vertex> even_vertex_pos;
    // and also for the odd vertices (that are inserted on an edge split)
    std::unordered_map<Edge*, Vertex> odd_vertex_pos;

    for (Vertex* x : *subdivided->vertices()) {
      if (x->IsBoundary()) {
        Vertex* a = nullptr;
        Vertex* b = nullptr;
        a = x->halfedge->vert;
        HalfEdge* curr = x->halfedge->next;
        while (curr->next->vert != x && !curr->IsBoundary()) {
          curr = curr->twin->next;
        }
        b = curr->vert;

        glm::vec3 new_pos = ((a->position + b->position) * 1.0F / 8.0F) +
                            (x->position * 3.0F / 4.0F);
        glm::vec3 new_norm =
            ((a->normal + b->normal) * 1.0F / 8.0F) + (x->normal * 3.0F / 4.0F);
        glm::vec2 new_uv = ((a->text_coords + b->text_coords) * 1.0F / 8.0F) +
                           (x->text_coords * 3.0F / 4.0F);
        Vertex n_v = Vertex(new_pos, new_norm, new_uv);
        even_vertex_pos[x] = n_v;
      } else {  // I am inside
        // this is simplified

        int k = x->Valence();
        float beta = 0.0F;
        if (k == 3) {
          beta = 3.0F / 16.0F;
        } else if (k > 3) {
          beta = 3.0F / (8.0F * static_cast<float>(k));
        } else {
          throw;  // unexpected valence (< 3 not a polygon)
        }

        glm::vec3 new_pos =
            (1.0F - (static_cast<float>(k) * beta)) * x->position;
        glm::vec3 new_norm = x->normal * (1.0F - static_cast<float>(k) * beta);
        glm::vec2 new_uv =
            x->text_coords * (1.0F - static_cast<float>(k) * beta);

        std::unordered_set<Vertex*> ring;
        const HalfEdge* curr = x->halfedge->twin->next;
        int counter = 1;
        while (curr != x->halfedge) {
          Vertex* b = curr->vert;
          assert(b != x);
          new_pos += (b->position * beta);
          new_norm += b->normal * beta;
          new_uv += b->text_coords * beta;
          curr = curr->twin->next;
          counter++;
        }
        new_pos += x->halfedge->vert->position * beta;
        new_norm += x->halfedge->vert->normal * beta;
        new_uv += x->halfedge->vert->text_coords * beta;
        assert(counter == x->Valence());
        even_vertex_pos[x] = Vertex(new_pos, new_norm, new_uv);
      }
    }

    for (Edge* e : *subdivided->edges()) {
      HalfEdge* h0 = e->halfedge;
      HalfEdge* h1 = e->halfedge->next;

      HalfEdge* h3 = e->halfedge->twin;
      HalfEdge* h4 = e->halfedge->twin->next;

      Vertex* v0 = h3->vert;
      Vertex* v1 = h1->vert;
      Vertex* v2 = h0->vert;
      Vertex* v3 = h4->vert;

      glm::vec3 new_pos =
          v2->position * 3.0F / 8.0F + v0->position * 3.0F / 8.0F +
          v1->position * 1.0F / 8.0F + v3->position * 1.0F / 8.0F;
      glm::vec3 new_norm = v2->normal * 3.0F / 8.0F + v0->normal * 3.0F / 8.0F +
                           v1->normal * 1.0F / 8.0F + v3->normal * 1.0F / 8.0F;
      glm::vec2 new_uv =
          v2->text_coords * 3.0F / 8.0F + v0->text_coords * 3.0F / 8.0F +
          v1->text_coords * 1.0F / 8.0F + v3->text_coords * 1.0F / 8.0F;
      odd_vertex_pos[e] = Vertex(new_pos, new_norm, new_uv);
    }

    const int edges_size = subdivided->edges()->size();
    // splitting the edges
    std::unordered_set<Vertex*> odd_vertices;
    std::unordered_set<Edge*> new_edges;

    for (int j = 0; j < edges_size; j++) {
      subdivided->split(subdivided->edges()->at(j),
                        odd_vertex_pos[subdivided->edges()->at(j)]);

      Vertex* x = subdivided->vertices()->back();
      odd_vertices.insert(x);
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
        if ((odd_vertices.find(e->halfedge->vert) != odd_vertices.end() &&
             odd_vertices.find(e->halfedge->twin->vert) ==
                 odd_vertices.end()) ||
            (odd_vertices.find(e->halfedge->vert) == odd_vertices.end() &&
             odd_vertices.find(e->halfedge->twin->vert) !=
                 odd_vertices.end())) {
          subdivided->flip(e);
        }
      }
    }

    // repositioning the even vertices
    for (auto& [v, n_v] : even_vertex_pos) {
      v->position = n_v.position;
      v->normal = n_v.normal;
      v->text_coords = n_v.text_coords;
    }
  }

  subdivided->GenerateOpenGLBuffers();
  return subdivided;
}
