#ifndef RENDERER_H
#define RENDERER_H

#include "scene.h"
#include <gl/glew.h>

// che cosa serve a un renderer?
// vettore(?) di luci
// quindi un renderer è composto da un vettore di luci ed una camera (btw sono
// tutte entità xdlolmao) e ha un loop che drawa tutti gli oggetti di una scena
// l'unica cosa che mi manca da capire come gestire sono le shader...?
class Renderer {
 public:
  Renderer();
  ~Renderer();
  void render(Scene scene);

  void toggleWireframe();

 private:
  // for wireframe
  GLint _gl_mode;
};

#endif  // RENDERER_H