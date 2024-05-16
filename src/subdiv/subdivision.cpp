#include "subdivision.h"

#include "../logger.h"

ISubdivision::~ISubdivision() {
  //
}

Mesh* LoopSubdiv::subdivide(const Model* in, int n_steps) {
  Mesh* curr = new Mesh(in->meshes()[0]);
  return curr;
}
