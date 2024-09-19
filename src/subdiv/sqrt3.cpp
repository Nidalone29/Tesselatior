#include "sqrt3.h"

#include <unordered_map>
#include <unordered_set>
#define _USE_MATH_DEFINES
#include <math.h>

#include "../logger.h"

// https://www.graphics.rwth-aachen.de/media/papers/sqrt31.pdf
TriMesh* Sqrt3Subdiv::subdivide(const TriMesh* in, int n_steps) {
  if (!in->IsManifold()) {
    // TODO deal with boundaries
    // ideally you should never be here
    throw;
  }

  const HalfEdgeData* hfd = in->half_edge_data();
  HalfEdgeData* subdivided = new HalfEdgeData(*hfd);

  // TODO MOVE THIS TO ISVALID() IN HALFEDGEDATA
  for (HalfEdge* he : *subdivided->half_edges()) {
    assert(he->Previous()->Previous()->Previous() == he);
  }

  for (int i = 0; i < n_steps; i++) {
    LOG_INFO("sqrt3 subdiv {}", i + 1);

    // computing the new vertex positions for the even vertices
    std::unordered_map<Face*, Vertex*> odd_vertices;
    std::unordered_map<Vertex*, Vertex> even_vertex_pos;
    even_vertex_pos.reserve(subdivided->vertices()->size());

    // compute even vertex positions
    for (Vertex* v : *subdivided->vertices()) {
      if (v->IsBoundary()) {
        // unsupported
        throw;
      } else {  // I am inside
        glm::vec3 new_pos = v->position;
        glm::vec2 new_uv = v->text_coords;  // idk if uv will work with sqrt3...
        const int valence = v->Valence();
        const float alpha = (4.0F - (2.0F * cos(2.0F * M_PI / valence))) / 9.0F;
        new_pos = (1 - alpha) * new_pos;
        new_uv = (1 - alpha) * new_uv;
        const HalfEdge* curr = v->halfedge;
        int counter = 0;
        do {
          Vertex* b = curr->vert;
          assert(b != v);
          new_pos += alpha * b->position * (1.0F / valence);
          // new_norm += b->normal * beta;
          new_uv += alpha * b->text_coords * (1.0F / valence);
          counter++;
          curr = curr->twin->next;
        } while (curr != v->halfedge);
        assert(counter == v->Valence());
        even_vertex_pos[v] = Vertex(new_pos, new_uv);
      }
    }

    for (Face* f : *subdivided->faces()) {
      Vertex* pi = f->halfedge->vert;  // this is the boundary 100%
      Vertex* pj = f->halfedge->next->vert;
      Vertex* pk = f->halfedge->next->next->vert;
      glm::vec3 newpos =
          (1.0F / 3.0F) * (pi->position + pj->position + pk->position);
      glm::vec2 newuv =
          (1.0F / 3.0F) * (pi->text_coords + pj->text_coords + pk->text_coords);
      odd_vertices[f] = new Vertex(newpos, newuv);
    }

    // now with this subdivision method we are rebuilding a new halfedge
    // mesh from scratch.
    // we keep track of the original edges, because we need to flip them later
    // (the non border ones)
    std::unordered_set<Edge*> original_edges;
    original_edges.reserve(subdivided->edges()->size());

    // it should be okay to do this "inplace" because this split operation does
    // not change the triangle borders
    const int original_faces_size = subdivided->faces()->size();
    for (int j = 0; j < original_faces_size; j++) {
      Face* f = subdivided->faces()->at(j);
      Vertex* new_v = odd_vertices[f];
      // we init
      HalfEdge* h0 = f->halfedge;
      HalfEdge* h1 = f->halfedge->next;
      HalfEdge* h2 = f->halfedge->next->next;
      HalfEdge* h3 = h0->twin;
      HalfEdge* h4 = h1->twin;
      HalfEdge* h5 = h2->twin;
      // external faces

      Vertex* v0 = h2->vert;
      Vertex* v1 = h0->vert;
      Vertex* v2 = h1->vert;

      Edge* e0 = h0->edge;
      Edge* e1 = h1->edge;
      Edge* e2 = h2->edge;

      // then we create the splitted face
      Face* f1 = new Face();
      Face* f2 = new Face();
      HalfEdge* h6 = new HalfEdge(f);
      HalfEdge* h7 = new HalfEdge(f);
      HalfEdge* h8 = new HalfEdge(f1);
      HalfEdge* h9 = new HalfEdge(f1);
      HalfEdge* h10 = new HalfEdge(f2);
      HalfEdge* h11 = new HalfEdge(f2);
      h0->face = f;
      h1->face = f1;
      h2->face = f2;

      // f
      h0->next = h6;
      h6->next = h7;
      h7->next = h0;

      // f1
      h1->next = h8;
      h8->next = h9;
      h9->next = h1;

      // f2
      h2->next = h10;
      h10->next = h11;
      h11->next = h2;

      // f
      h6->vert = new_v;
      h7->vert = v0;
      h0->vert = v1;
      // f1
      h8->vert = new_v;
      h9->vert = v1;
      h1->vert = v2;
      // f2
      h2->vert = v0;
      h10->vert = new_v;
      h11->vert = v2;

      v0->halfedge = h0;
      v1->halfedge = h1;
      v2->halfedge = h2;
      new_v->halfedge = h7;

      f->halfedge = h0;
      f1->halfedge = h1;
      f2->halfedge = h2;

      Edge* e3 = new Edge();
      Edge* e4 = new Edge();
      Edge* e5 = new Edge();
      e3->halfedge = h7;
      e4->halfedge = h9;
      e5->halfedge = h11;

      h0->edge = e0;
      h1->edge = e1;
      h2->edge = e2;
      /*
      if (h3 != nullptr) {
        h3->edge = e0;
      }
      if (h4 != nullptr) {
        h4->edge = e1;
      }
      if (h5 != nullptr) {
        h5->edge = e2;
      }
      */

      h6->edge = e4;
      h7->edge = e3;
      h8->edge = e5;
      h9->edge = e4;
      h10->edge = e3;
      h11->edge = e5;

      h7->twin = h10;
      h6->twin = h9;
      h8->twin = h11;

      h10->twin = h7;
      h9->twin = h6;
      h11->twin = h8;

      subdivided->vertices()->push_back(new_v);
      subdivided->faces()->push_back(f1);
      subdivided->faces()->push_back(f2);
      subdivided->half_edges()->push_back(h6);
      subdivided->half_edges()->push_back(h7);
      subdivided->half_edges()->push_back(h8);
      subdivided->half_edges()->push_back(h9);
      subdivided->half_edges()->push_back(h10);
      subdivided->half_edges()->push_back(h11);
      subdivided->edges()->push_back(e3);
      subdivided->edges()->push_back(e4);
      subdivided->edges()->push_back(e5);
      original_edges.insert(e0);
      original_edges.insert(e1);
      original_edges.insert(e2);
    }

    for (Edge* e : original_edges) {
      if (!e->halfedge->IsBoundary()) {
        flip(subdivided, e);
      }
    }

    // repositioning the even vertices
    for (auto& [v, n_v] : even_vertex_pos) {
      v->position = n_v.position;
      // v->normal = n_v.normal;
      v->text_coords = n_v.text_coords;
    }
  }

  TriMesh* output = new TriMesh(subdivided, in->material());
  output->GenerateOpenGLBuffers();
  return output;
}

void Sqrt3Subdiv::flip(HalfEdgeData* m, const Edge* e) {
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