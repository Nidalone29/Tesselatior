#ifndef SUBDIVISION_H
#define SUBDIVISION_H

#include <map>
#include <string>

#include "../model.h"

// sa stands for subdivision algorithms
namespace sa {

enum class SubDiv {
  NONE,
  // --
  LOOP,
  CATMULL,
  SQRT3
};

// why inline and not static?
// https://www.youtube.com/watch?v=QVHwOOrSh3w
// https://www.youtube.com/watch?v=rQhBECyA6ew
// string for displaying algorithms name to UI
inline const std::map<SubDiv, std::string> kSubdivisions = {
    {SubDiv::NONE, "none"},
    {SubDiv::LOOP, "loop"},
    {SubDiv::CATMULL, "catmull"},
    {SubDiv::SQRT3, "sqrt3"},
};

};  // namespace sa

// [chapter 17.5 in RealTimeRendering 4th edition]
// Subdivision in two phases
// 1. Refinement phase: Given a control mesh create new vertices and reconnects
// them to form smaller primitives
// 2. Smoothing phase: Compute new positions for some (or all) the vertices
class ISubdivision {
 public:
  virtual ~ISubdivision() = 0;

  virtual Mesh* subdivide(const Model* in, int n_steps) = 0;

 private:
};

class LoopSubdiv : public ISubdivision {
 public:
  Mesh* subdivide(const Model* in, int n_steps) override;

 private:
};

#endif  // SUBDIVISION_H