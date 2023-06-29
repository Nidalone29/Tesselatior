#ifndef TEAPOT_H
#define TEAPOT_H

#include "mesh.h"

class Teapot {
 public:
  Teapot();

  void render();

  void init();

 private:
  Mesh _model;
  bool _initialized;

  Teapot& operator=(const Teapot& other);
  Teapot(const Teapot& other);
};

#endif  // TEAPOT_H