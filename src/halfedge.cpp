#include "halfedge.h"

#include <unordered_map>
#include <unordered_set>

#include "logger.h"
#include "utilities.h"

HalfEdgeData::HalfEdgeData(std::vector<Vertex*>* vertices,
                           std::vector<HalfEdge*>* halfedges,
                           std::vector<Face*>* faces, std::vector<Edge*>* edges)
    : vertices_(vertices),
      half_edges_(halfedges),
      faces_(faces),
      edges_(edges) {
  LOG_TRACE(
      "HalfEdgeData(std::vector<Vertex>*, std::vector<HalfEdge*>*, "
      "std::vector<Face*>*, std::vector<Edge*>*, std::vector<unsigned "
      "int>*)");
}

HalfEdgeData::HalfEdgeData(const HalfEdgeData& other) {
  LOG_TRACE("HalfEdgeData::HalfEdgeData(const HalfEdgeData&)");

  vertices_ = new std::vector<Vertex*>(/* other.vertices_->size() */);
  half_edges_ = new std::vector<HalfEdge*>(/* other.half_edges_->size() */);
  faces_ = new std::vector<Face*>(/* other.faces_->size() */);
  edges_ = new std::vector<Edge*>(/* other.edges_->size() */);

  // we basically map the other data to the new one
  // OTN ===> old to new
  // TODO this can probably be reimplemented using only indices instead of
  // hashmaps, improving the memory complexity from O(n) to O(1)
  std::unordered_map<HalfEdge*, HalfEdge*> OTN_halfedge;
  std::unordered_map<Edge*, Edge*> OTN_edge;
  std::unordered_map<Face*, Face*> OTN_face;
  std::unordered_map<Vertex*, Vertex*> OTN_vert;

  for (HalfEdge* ohe : *other.half_edges_) {
    HalfEdge* he = new HalfEdge();
    OTN_halfedge[ohe] = he;
  }
  for (auto [ohe, nhe] : OTN_halfedge) {
    nhe->next = OTN_halfedge[ohe->next];
    nhe->twin = OTN_halfedge[ohe->twin];
    // we still don't have the other data
  }

  // now that we have initial halfedges we can do the edges, faces and vertices
  for (Edge* oe : *other.edges_) {
    Edge* e = new Edge();
    e->halfedge = OTN_halfedge[oe->halfedge];
    OTN_edge[oe] = e;
  }

  for (Face* of : *other.faces_) {
    Face* f = new Face();
    f->halfedge = OTN_halfedge[of->halfedge];
    OTN_face[of] = f;
  }

  for (Vertex* ov : *other.vertices_) {
    Vertex* v = new Vertex(ov->position, ov->normal, ov->text_coords);
    v->halfedge = OTN_halfedge[ov->halfedge];
    OTN_vert[ov] = v;
  }

  for (auto [ohe, nhe] : OTN_halfedge) {
    // now we can finish the edges
    nhe->face = OTN_face[ohe->face];
    nhe->edge = OTN_edge[ohe->edge];
    nhe->vert = OTN_vert[ohe->vert];
  }

  for (auto [_, v] : OTN_vert) {
    vertices_->push_back(v);
  }
  for (auto [_, e] : OTN_edge) {
    edges_->push_back(e);
  }
  for (auto [_, f] : OTN_face) {
    faces_->push_back(f);
  }
  for (auto [_, hf] : OTN_halfedge) {
    half_edges_->push_back(hf);
  }
}

HalfEdgeData& HalfEdgeData::operator=(const HalfEdgeData& other) {
  LOG_TRACE("HalfEdgeData& operator=(const HalfEdgeData& other)");

  if (this != &other) {
    HalfEdgeData temp(other);
    std::swap(this->vertices_, temp.vertices_);
    std::swap(this->half_edges_, temp.half_edges_);
    std::swap(this->faces_, temp.faces_);
    std::swap(this->edges_, temp.edges_);
  }

  return *this;
}

HalfEdgeData::~HalfEdgeData() {
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
}

// TODO IMPLEMENT THIS
bool HalfEdgeData::IsValid() const {
  // somewhere here
  return true;
}

bool HalfEdgeData::IsValidType(MESH_TYPE type) const {
  if (type == MESH_TYPE::POLY) {
    return true;
  }

  const int expected = to_underlying(type);
  for (Face* f : *faces_) {
    int count = 0;
    HalfEdge* start = f->halfedge;
    HalfEdge* curr = start;

    do {
      count++;
      curr = curr->next;
    } while (curr != start);

    if (count != expected) {
      return false;
    }
  }
  return true;
}

std::vector<Vertex*>* HalfEdgeData::vertices() {
  return vertices_;
}

std::vector<HalfEdge*>* HalfEdgeData::half_edges() {
  return half_edges_;
}

std::vector<Face*>* HalfEdgeData::faces() {
  return faces_;
}

std::vector<Edge*>* HalfEdgeData::edges() {
  return edges_;
}

const std::vector<Vertex*>* HalfEdgeData::vertices() const {
  return vertices_;
}

const std::vector<HalfEdge*>* HalfEdgeData::half_edges() const {
  return half_edges_;
}

const std::vector<Face*>* HalfEdgeData::faces() const {
  return faces_;
}

const std::vector<Edge*>* HalfEdgeData::edges() const {
  return edges_;
}

// computing smooth normals using the optimization explained in
// https://iquilezles.org/articles/normals/
void HalfEdgeData::ShadeSmooth() {
  for (const Face* f : *faces_) {
    const glm::vec3 face_normal = f->ComputeNormalWithArea();

    // visit every vertex in the face and update it's normal with the current
    // face normal contribution
    const HalfEdge* f_start = f->halfedge;
    HalfEdge* f_curr = f->halfedge;
    do {
      Vertex* v = f_curr->vert;

      if (v->IsBoundary()) {
        HalfEdge* he = v->halfedge;

        // visit counter-clockwise to get the first halfedge on the left
        he = v->halfedge->Previous();
        while (!he->IsBoundary()) {
          he = he->twin->Previous();
        }

        // and now we explore the faces from left to right (clockwise)
        HalfEdge* curr = he->next;
        do {
          v->normal += face_normal;
          curr = curr->twin->next;
        } while (!curr->IsBoundary());
      } else {  // I am inside
        HalfEdge* curr = v->halfedge;

        do {
          v->normal += face_normal;
          curr = curr->twin->next;
        } while (curr != v->halfedge);
      }

      f_curr = f_curr->next;
    } while (f_start != f_curr);
  }

  // now normalize at the end so we have the areas contribution
  for (Vertex* v : *vertices_) {
    v->normal = glm::normalize(v->normal);
  }
}

// ---

// you have the responsibility to delete the vector
// TODO optimizable with RVO?
std::vector<unsigned int>* CreateIndexBuffer(const HalfEdgeData* hf_data) {
  std::vector<unsigned int>* index_buffer = new std::vector<unsigned int>();

  std::unordered_map<Vertex*, unsigned int> added_verts;

  int i = 0;
  for (const Face* x : *hf_data->faces()) {
    HalfEdge* start = x->halfedge;
    HalfEdge* curr = start;

    do {
      if (added_verts.find(curr->vert) == added_verts.end()) {
        added_verts[curr->vert] = i;
        i++;
      }
      index_buffer->push_back(added_verts[curr->vert]);
      curr = curr->next;
    } while (curr != start);
  }

  return index_buffer;
}

// you have the responsibility to delete the vector
// TODO optimizable with RVO?
std::vector<Vertex>* CreateVertexBuffer(const HalfEdgeData* hf_data) {
  std::vector<Vertex>* vertex_buffer = new std::vector<Vertex>();
  vertex_buffer->reserve(hf_data->vertices()->size());

  std::unordered_set<Vertex*> added_verts;

  for (const Face* x : *hf_data->faces()) {
    HalfEdge* start = x->halfedge;
    HalfEdge* curr = start;

    do {
      if (added_verts.find(curr->vert) == added_verts.end()) {
        added_verts.insert(curr->vert);
        vertex_buffer->push_back(*curr->vert);
      }
      curr = curr->next;
    } while (curr != start);
  }

  return vertex_buffer;
}