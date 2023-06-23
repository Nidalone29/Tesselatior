#ifndef FLOWER_H
#define FLOWER_H

#include "mesh.h"

class Flower {
 public:
  Flower();

  void render(void);

  void init(void);

 private:
  Mesh _model;
  bool _initialized;

  Flower& operator=(const Flower& other);
  Flower(const Flower& other);
};

#endif