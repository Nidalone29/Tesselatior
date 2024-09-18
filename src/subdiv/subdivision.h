#ifndef SUBDIVISION_H
#define SUBDIVISION_H

#include "../mesh.h"

// [chapter 17.5 in RealTimeRendering 4th edition]
// Subdivision in two phases
// 1. Refinement phase: Given a control mesh create new vertices and reconnects
// them to form smaller primitives
// 2. Smoothing phase: Compute new positions for some (or all) the vertices
class ISubdivision {
 public:
  virtual ~ISubdivision() = 0;

  [[nodiscard]] virtual IMesh* subdivide(IMesh* in, int n_steps) = 0;

 private:
};

class NoneSubdiv final : public ISubdivision {
 public:
  virtual ~NoneSubdiv();

  [[nodiscard]] IMesh* subdivide(IMesh* in, int n_steps) override;

 private:
};

#endif  // SUBDIVISION_H