#ifndef BOOT_H
#define BOOT_H

#include "mesh.h"

class Boot {
 public:
  Boot();

  void render(void);

  void init(void);

 private:
  Mesh _model;
  bool _initialized;

  Boot& operator=(const Boot& other);
  Boot(const Boot& other);
};

#endif