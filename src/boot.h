#ifndef BOOT_H
#define BOOT_H

#include "mesh.h"

class Boot {
 public:
  Boot();

  void render();

  void init();

 private:
  Mesh _model;
  bool _initialized;

  Boot& operator=(const Boot& other);
  Boot(const Boot& other);
};

#endif  // BOOT_H