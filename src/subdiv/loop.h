#ifndef LOOP_H
#define LOOP_H

#include "subdivision.h"

class LoopSubdiv final : public ISubdivision {
 public:
  // I can't specify the argument type because it violates the Liskov
  // Substitution Principle
  [[nodiscard]] TriMesh* subdivide(IMesh* in, int n_steps) override {
    if (TriMesh* d = dynamic_cast<TriMesh*>(in); d != nullptr) {
      // the architecture of the program
      // (AvailableSubdivAlgosFactory::GetAvailableAlgos()) should guarantee
      // that this works
      return subdivide(d, n_steps);
    }
    return nullptr;
  }

  [[nodiscard]] TriMesh* subdivide(TriMesh* in, int n_steps);

 private:
  /**
   * @brief this modifies the mesh m (by adding data such as vertices, faces,
   * halfedges...)
   * @param e edge to be split
   * @param new_vert the position of the new vertex in the split (along the
   * edge)
   */
  void split(HalfEdgeData* m, Edge* e, const Vertex& new_vert);
  /**
   * @brief flips an edge (does not add any data to the mesh)
   * @param e edge to flip
   */
  void flip(HalfEdgeData* m, const Edge* e);
};

#endif  // LOOP_H