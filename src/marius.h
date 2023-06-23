#ifndef MARIUS_H
#define MARIUS_H

#include "mesh.h"

class Marius {
 public:
  Marius();

  void render(void);

  void init(void);

 private:
  Mesh _head, _eyes, _hairplate, _eyebrows;
  Mesh _eyelashesUpper, _eyelashesLower;
  bool _initialized;

  Marius& operator=(const Marius& other);
  Marius(const Marius& other);
};

#endif