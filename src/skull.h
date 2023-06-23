#ifndef SKULL_H
#define SKULL_H

#include "mesh.h"

class Skull {
 public:
  Skull();

  void render(void);

  void init(void);

 private:
  Mesh _model;
  bool _initialized;

  Skull& operator=(const Skull& other);
  Skull(const Skull& other);
};

#endif