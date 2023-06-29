#ifndef DRAGON_H
#define DRAGON_H

#include "mesh.h"

class Dragon {
 public:
  Dragon();

  void render();

  void init();

 private:
  Mesh _model;
  bool _initialized;

  Dragon& operator=(const Dragon& other);
  Dragon(const Dragon& other);
};

#endif  // DRAGON_H