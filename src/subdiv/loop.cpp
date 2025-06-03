#include "loop.h"

#include <unordered_map>
#include <unordered_set>

#include "../logger.h"

// loosely inspired by https://github.com/cmu462/Scotty3D/wiki/Loop-Subdivision
// and chapter 4.2 of
// https://graphics.stanford.edu/courses/cs348a-09-fall/Papers/zorin-subdivision00.pdf
TriMesh* LoopSubdiv::subdivide(TriMesh* in, int n_steps) {
  const HalfEdgeData* hfd = in->half_edge_data();
  HalfEdgeData* subdivided = new HalfEdgeData(*hfd);

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
        // find the first one clockwise
        HalfEdge* he = x->halfedge;
        while (!he->IsBoundary()) {
          he = he->twin->next;
        }
        a = he->vert;
        // find the other one counter-clockwise
        he = x->halfedge->Previous();
        while (!he->IsBoundary()) {
          he = he->twin->Previous();
        }
        b = he->Previous()->vert;

        glm::vec3 new_pos = ((a->position + b->position) * 1.0F / 8.0F) +
                            (x->position * 3.0F / 4.0F);
        glm::vec2 new_uv = ((a->text_coords + b->text_coords) * 1.0F / 8.0F) +
                           (x->text_coords * 3.0F / 4.0F);
        Vertex n_v = Vertex(new_pos, new_uv);

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
        glm::vec2 new_uv =
            x->text_coords * (1.0F - static_cast<float>(k) * beta);

        const HalfEdge* curr = x->halfedge;
        int counter = 0;
        do {
          Vertex* b = curr->vert;
          assert(b != x);
          new_pos += (b->position * beta);
          // new_norm += b->normal * beta;
          new_uv += b->text_coords * beta;
          counter++;
          curr = curr->twin->next;
        } while (curr != x->halfedge);
        assert(counter == x->Valence());

        even_vertex_pos[x] = Vertex(new_pos, new_uv);
      }
    }

    for (Edge* e : *subdivided->edges()) {
      if (!e->halfedge->IsBoundary()) {
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
        glm::vec2 new_uv =
            v2->text_coords * 3.0F / 8.0F + v0->text_coords * 3.0F / 8.0F +
            v1->text_coords * 1.0F / 8.0F + v3->text_coords * 1.0F / 8.0F;
        odd_vertex_pos[e] = Vertex(new_pos, new_uv);
      } else {
        Vertex* a = e->halfedge->vert;
        Vertex* b = e->halfedge->next->next->vert;

        glm::vec3 new_pos =
            a->position * 1.0F / 2.0F + b->position * 1.0F / 2.0F;
        // glm::vec3 new_norm = a->normal * 1.0F / 2.0F + b->normal * 1.0F
        // / 2.0F;
        glm::vec2 new_uv =
            a->text_coords * 1.0F / 2.0F + b->text_coords * 1.0F / 2.0F;
        odd_vertex_pos[e] = Vertex(new_pos, new_uv);
      }
    }

    const int edges_size = subdivided->edges()->size();
    // splitting the edges
    std::unordered_set<Vertex*> odd_vertices;
    std::unordered_set<Edge*> new_edges;

    for (int j = 0; j < edges_size; j++) {
      split(subdivided, subdivided->edges()->at(j),
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
          flip(subdivided, e);
        }
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
  return output;
}

void LoopSubdiv::split(HalfEdgeData* m, Edge* e, const Vertex& new_vert) {
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

    m->vertices()->push_back(v3);
    v3->halfedge = h3;

    f1->halfedge = h1;
    f0->halfedge = h0;

    h4->vert = v3;
    h5->vert = v0;
    h1->vert = v1;

    h3->vert = v1;
    h2->vert = v2;
    h0->vert = v3;

    m->faces()->push_back(f1);
    m->half_edges()->push_back(h3);
    m->half_edges()->push_back(h4);
    m->half_edges()->push_back(h5);
    m->edges()->push_back(e3);
    m->edges()->push_back(e4);
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

    Edge* e1 = h2->edge;
    Edge* e2 = h1->edge;
    Edge* e3 = h5->edge;
    Edge* e4 = h4->edge;

    HalfEdge* t1 = h2->twin;
    HalfEdge* t2 = h1->twin;
    HalfEdge* t3 = h5->twin;
    HalfEdge* t4 = h4->twin;

    // assert(e1 == t1->edge);
    // assert(e2 == t2->edge);
    // assert(e3 == t3->edge);
    // assert(e4 == t4->edge);

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
    if (t1 != nullptr) {
      h2->twin = t1;
      t1->twin = h2;
    }
    // e2
    if (t2 != nullptr) {
      h1->twin = t2;
      t2->twin = h1;
    }
    // e3
    if (t3 != nullptr) {
      h5->twin = t3;
      t3->twin = h5;
    }
    // e4
    if (t4 != nullptr) {
      h4->twin = t4;
      t4->twin = h4;
    }
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
    // assert(t1->vert == v1);
    // assert(t2->vert == v2);
    // assert(t3->vert == v3);
    // assert(t4->vert == v0);

    m->vertices()->push_back(v4);
    m->faces()->push_back(f2);
    m->faces()->push_back(f3);
    m->half_edges()->push_back(h6);
    m->half_edges()->push_back(h7);
    m->half_edges()->push_back(h8);
    m->half_edges()->push_back(h9);
    m->half_edges()->push_back(h10);
    m->half_edges()->push_back(h11);

    // this edge is the other half of the original edge
    m->edges()->push_back(e6);

    // and these ones are new
    m->edges()->push_back(e5);
    m->edges()->push_back(e7);
  }
}

void LoopSubdiv::flip(HalfEdgeData* m, const Edge* e) {
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