#include "catmullclark.h"

#include "../logger.h"

// TODO adapt for any mesh in input
// based on
// https://en.wikipedia.org/wiki/Catmull%E2%80%93Clark_subdivision_surface
QuadMesh* CatmullClarkSubdiv::subdivide(const QuadMesh* in, int n_steps) {
  if (!in->IsManifold()) {
    // TODO deal with boundaries
    // ideally you should never be here
    LOG_ERROR("MESH IS NOT MANIFOLD");
    throw;
  }

  const HalfEdgeData* hfd = in->half_edge_data();
  HalfEdgeData* subdivided = new HalfEdgeData(*hfd);

  // implementation here
  for (int i = 0; i < n_steps; i++) {
    // new face point at the center
    std::unordered_map<Face*, Vertex*> new_face_points;
    for (Face* f : *subdivided->faces()) {
      Vertex* new_v =
          new Vertex(glm::vec3(0.0F, 0.0F, 0.0F), glm::vec2(0.0F, 0.0F));

      const HalfEdge* start = f->halfedge;
      HalfEdge* curr = f->halfedge;
      int counter = 0;
      do {
        new_v->position += curr->vert->position;
        new_v->text_coords += curr->vert->text_coords;
        counter++;
        curr = curr->next;
      } while (start != curr);
      new_v->position /= counter;
      new_v->text_coords /= counter;
      new_face_points[f] = new_v;
    }
    // new edge midpoints
    std::unordered_map<Edge*, Vertex*> new_edge_points;
    for (Edge* e : *subdivided->edges()) {
      Vertex* new_v =
          new Vertex(glm::vec3(0.0F, 0.0F, 0.0F), glm::vec2(0.0F, 0.0F));
      // the two edge points
      Vertex* v_e1 = e->halfedge->vert;
      Vertex* v_e2 = e->halfedge->twin->vert;
      // the two face points
      Vertex* v_f1 = new_face_points[e->halfedge->face];
      Vertex* v_f2 = new_face_points[e->halfedge->twin->face];
      new_v->position = (v_e1->position + v_e2->position +  //
                         v_f1->position + v_f2->position) /
                        4.0F;
      new_v->text_coords = (v_e1->text_coords + v_e2->text_coords +
                            v_f1->text_coords + v_f2->text_coords) /
                           4.0F;
      new_edge_points[e] = new_v;
    }
    // update old vertices positions
    for (Vertex* v : *subdivided->vertices()) {
      const float n = static_cast<float>(v->Valence());
      glm::vec3 f_pos = {0.0F, 0.0F, 0.0F};
      glm::vec2 f_uv = {0.0F, 0.0F};
      const HalfEdge* curr = v->halfedge;
      // go around the face and avg the new points
      do {
        const Vertex* b = new_face_points[curr->face];
        assert(b != v);
        f_pos += b->position;
        f_uv += b->text_coords;
        curr = curr->twin->next;
      } while (curr != v->halfedge);
      f_pos /= n;
      f_uv /= n;

      // go around and avg the midpoints of the edges (we are not using the new
      // edgepoints for now)
      glm::vec3 r_pos = {0.0F, 0.0F, 0.0F};
      glm::vec2 r_uv = {0.0F, 0.0F};
      curr = v->halfedge;
      // go around the face and avg the new points
      do {
        const Vertex* a = curr->vert;
        const Vertex* b = curr->twin->vert;
        r_pos += (a->position + b->position) / 2.0F;
        r_uv += (a->text_coords + b->text_coords) / 2.0F;
        curr = curr->twin->next;
      } while (curr != v->halfedge);
      r_pos /= n;
      r_uv /= n;

      v->position = (f_pos + 2.0F * r_pos + (n - 3.0F) * v->position) / n;
      v->text_coords = (f_uv + 2.0F * r_uv + (n - 3.0F) * v->text_coords) / n;
    }

    // first we split all the edges by the midpoints
    std::unordered_map<HalfEdge*, Edge*> halfedge_to_original_edge;
    const int edges_size = subdivided->edges()->size();
    for (int j = 0; j < edges_size; j++) {
      Edge* e = subdivided->edges()->at(j);
      HalfEdge* h1 = e->halfedge;
      HalfEdge* h2 = e->halfedge->twin;
      Vertex* a = h2->vert;
      Vertex* b = h1->vert;
      Vertex* m = new_edge_points[e];
      Face* f1 = h1->face;
      Face* f2 = h2->face;

      Edge* e1 = new Edge();
      HalfEdge* h3 = new HalfEdge(f1);
      HalfEdge* h4 = new HalfEdge(f2);
      e1->halfedge = h4;

      HalfEdge* n1 = h1->next;
      HalfEdge* n2 = h1->Previous();
      HalfEdge* n3 = h2->next;
      HalfEdge* n4 = h2->Previous();

      h1->twin = h2;
      h2->twin = h1;
      h3->twin = h4;
      h4->twin = h3;
      h3->edge = e1;
      h4->edge = e1;
      h1->edge = e;
      h2->edge = e;

      // this is just temporary, will update next loop
      // h3->next = h1;
      // h1->next = h1->next;
      // h4->next = h2->next;
      // h2->next = h4;

      h1->next = n1;
      h2->next = h4;
      h3->next = h1;
      h4->next = n3;

      n2->next = h3;
      n4->next = h2;

      h1->vert = b;
      h2->vert = m;
      h3->vert = m;
      h4->vert = a;
      m->halfedge = h1;

      // WE NEED TO SET THE HALFEDGE TO THE ONE THAT POINTS TO THE CORNER
      // (right) SO THAT NEXT LOOP WORKS
      f1->halfedge = h1;
      f2->halfedge = h4;

      halfedge_to_original_edge[h1] = e;
      halfedge_to_original_edge[h2] = e;
      halfedge_to_original_edge[h3] = e;
      halfedge_to_original_edge[h4] = e;

      subdivided->edges()->push_back(e1);
      subdivided->half_edges()->push_back(h3);
      subdivided->half_edges()->push_back(h4);
    }

    // at this point the halfedge structure is invalid (we have not set the
    // nexts), but we will fix it with the next loop
    const int original_faces_size = subdivided->faces()->size();
    std::vector<Face*>* new_subdivided_faces = new std::vector<Face*>();
    new_subdivided_faces->reserve(original_faces_size * 4);

    for (int j = 0; j < original_faces_size; j++) {
      Face* f = subdivided->faces()->at(j);
      struct FaceData {
        Vertex* corner_vert;
        Vertex* mid_vert;
        HalfEdge* from_midpoint;
        HalfEdge* from_corner;
      };
      Vertex* v8 = new_face_points[f];

      HalfEdge* curr = f->halfedge;
      std::vector<FaceData> face_data_vert;
      do {
        FaceData fdf = {
            curr->vert,                                           // corner_vert
            new_edge_points[halfedge_to_original_edge.at(curr)],  // mid_vert
            curr,         // from_midpoint
            curr->next};  // from_corner
        face_data_vert.emplace_back(fdf);

        curr = curr->next->next;
      } while (curr != f->halfedge);
      // assert(face_data_vert.size() == 4);

      // now we build the new structure in place in the face, and we update
      // everything except the twins on the original edges
      // we will delete the 4 original halfedges and edges of the face and just
      // create new ones
      std::vector<Face*> new_faces;
      for (int k = 0; k < face_data_vert.size(); k++) {
        Face* new_f = new Face();
        new_faces.push_back(new_f);
        HalfEdge* h0 = face_data_vert[k].from_midpoint;
        HalfEdge* h1 = face_data_vert[k].from_corner;
        HalfEdge* h2 = new HalfEdge(new_f);
        HalfEdge* h3 = new HalfEdge(new_f);
        // outer
        Edge* e0 = face_data_vert[k].from_midpoint->edge;
        Edge* e1 = face_data_vert[k].from_corner->edge;
        assert(e0 != e1);
        // inner
        // Edge* e2 = new Edge();
        // Edge* e3 = new Edge();

        Vertex* v0 = face_data_vert[k].mid_vert;
        Vertex* v1 = face_data_vert[k].corner_vert;
        Vertex* v2 = face_data_vert[(k + 1) % face_data_vert.size()].mid_vert;
        Vertex* v3 = v8;

        h0->vert = v1;
        h1->vert = v2;
        h2->vert = v3;
        h3->vert = v0;

        h0->next = h1;
        h1->next = h2;
        h2->next = h3;
        h3->next = h0;

        // h0->edge = e0;
        // h1->edge = e1;
        // h2->edge = e2;
        // h3->edge = e3;

        v0->halfedge = h0;
        v1->halfedge = h1;
        v2->halfedge = h2;
        v3->halfedge = h3;

        // e0->halfedge = h0;
        // e1->halfedge = h1;
        // e2->halfedge = h2;
        // e3->halfedge = h3;

        new_f->halfedge = h2;

        h0->face = new_f;
        h1->face = new_f;

        new_subdivided_faces->push_back(new_f);

        subdivided->half_edges()->push_back(h2);
        subdivided->half_edges()->push_back(h3);
        // subdivided->edges()->push_back(e2);
        // subdivided->edges()->push_back(e3);
      }

      // and now we have to deal with the internal edges and the twins
      for (int k = 0; k < new_faces.size(); k++) {
        Face* local_f = new_faces[k];
        Face* local_f_succ = new_faces[(k + 1) % new_faces.size()];

        HalfEdge* h2 = local_f->halfedge;
        HalfEdge* h3 = local_f->halfedge->next;
        HalfEdge* h2_succ = local_f_succ->halfedge;
        HalfEdge* h3_succ = local_f_succ->halfedge->next;

        h2->twin = h3_succ;
        h3_succ->twin = h2;
        Edge* new_e = new Edge();
        new_e->halfedge = h2;
        h2->edge = new_e;
        h3_succ->edge = new_e;
        subdivided->edges()->push_back(new_e);
      }
    }

    // add the midpoint vertices
    for (const auto [_, v] : new_edge_points) {
      subdivided->vertices()->push_back(v);
    }
    // add the face midpoint vertices
    for (const auto [_, v] : new_face_points) {
      subdivided->vertices()->push_back(v);
    }

    // improvable
    for (Face* f : *subdivided->faces()) {
      delete f;
    }
    delete subdivided->faces();
    subdivided->faces(new_subdivided_faces);
  }

  QuadMesh* output = new QuadMesh(subdivided, in->material());
  return output;
}
