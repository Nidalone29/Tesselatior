#include "subdivision.h"

ISubdivision::~ISubdivision() {
  //
}

NoneSubdiv::~NoneSubdiv() {
  //
}

IMesh* NoneSubdiv::subdivide(IMesh* in, int n_steps) {
  return in->clone();
}
