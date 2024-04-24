#ifndef SUBDIVISION_H
#define SUBDIVISION_H

#include <map>
#include <string>

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

class ISubdivision {
 public:
  virtual ~ISubdivision() = 0;

  // virtual Model subdivide(StaticModel in, int n_steps) = 0;

 private:
};

#endif  // SUBDIVISION_H