#ifndef SQRT3_H
#define SQRT3_H

#include "subdivision.h"

class Sqrt3Subdiv final : public ISubdivision {
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
   * @brief flips an edge (does not add any data to the mesh)
   * @param e edge to flip
   */
  void flip(HalfEdgeData* m, const Edge* e);
};

#endif  // SQRT3_H