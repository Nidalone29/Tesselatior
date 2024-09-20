#ifndef HALFEDGE_H
#define HALFEDGE_H

#include <vector>

#include "vertex.h"

enum class MESH_TYPE {
  TRI = 3,
  QUAD = 4,
  POLY = 0,
};

// this data structure hold all the topological information for a mesh in
// halfedge form
class HalfEdgeData {
 public:
  HalfEdgeData() = delete;
  HalfEdgeData(std::vector<Vertex*>* vertices,
               std::vector<HalfEdge*>* halfedges, std::vector<Face*>* faces,
               std::vector<Edge*>* edges);
  // HalfEdgeData(const std::vector<Vertex*>& vertices, std::vector<unsigned
  // int>* indices);
  HalfEdgeData(const HalfEdgeData& other);
  HalfEdgeData& operator=(const HalfEdgeData& other);

  // TODO support move semantics
  HalfEdgeData(HalfEdgeData&& other) = delete;
  HalfEdgeData& operator=(HalfEdgeData&& other) = delete;

  ~HalfEdgeData();

  void Clear();

  // expensive ([[nodiscard]] is necessary) function that checks if this
  // halfedge data is valid (meaning no dangling vertices, all halfedge setted
  // correctly...)
  // Intended for use for debug purposes
  [[nodiscard]] bool IsValid() const;
  // validate mesh in O(n)
  bool IsValidType(MESH_TYPE type = MESH_TYPE::POLY) const;

  [[nodiscard]] std::vector<Vertex*>* vertices();
  [[nodiscard]] std::vector<HalfEdge*>* half_edges();
  [[nodiscard]] std::vector<Face*>* faces();
  void faces(std::vector<Face*>* new_faces);
  [[nodiscard]] std::vector<Edge*>* edges();
  [[nodiscard]] const std::vector<Vertex*>* vertices() const;
  [[nodiscard]] const std::vector<HalfEdge*>* half_edges() const;
  [[nodiscard]] const std::vector<Face*>* faces() const;
  [[nodiscard]] const std::vector<Edge*>* edges() const;

  void ShadeSmooth();

  bool IsManifold() const;

 private:
  std::vector<Vertex*>* vertices_;
  std::vector<HalfEdge*>* half_edges_;
  std::vector<Face*>* faces_;
  std::vector<Edge*>* edges_;
};

std::vector<unsigned int>* CreateIndexBuffer(const HalfEdgeData* hf_data);
std::vector<Vertex>* CreateVertexBuffer(const HalfEdgeData* hf_data);

#endif  // HALFEDGE_H
