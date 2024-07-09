#ifndef LOOP_H
#define LOOP_H

#include "subdivision.h"

class LoopSubdiv : public ISubdivision {
 public:
  TriMesh* subdivide(const Model* in, int n_steps) override;

  /**
   * @brief this modifies the mesh m (by adding data such as vertices, faces,
   * halfedges...)
   * @param e edge to be split
   * @param new_vert the position of the new vertex in the split (along the
   * edge)
   */
  void split(TriMesh* m, Edge* e, const Vertex& new_vert);
  /**
   * @brief flips an edge (does not add any data to the mesh)
   * @param e edge to flip
   */
  void flip(TriMesh* m, const Edge* e);

 private:
};

#endif  // LOOP_H