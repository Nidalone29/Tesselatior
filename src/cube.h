#ifndef CUBE_H
#define CUBE_H

#include "GL/glew.h"  // prima di freeglut
#include "texture2D.h"

class Cube {
 public:
  /**
    Costruttore
  */
  Cube();

  /**
    Renderizza l'oggetto. Se necessario, l'oggetto viene prima inizializzato
    chiamando la init().
  */
  void render(void);

  /**
    Inizializzazione degli oggetti OpenGL. E' fatta in un secondo tempo
    poichè è necessario avere inizializzato GLUT prima di qualunque
    uso delle funzioni di OpenGL. La init è chiamata, se necessario,
    dalla render().
  */
  void init(void);

 private:
  GLuint _VAO;               // Vertex Array Object
  Texture2D _color_texture;  // Color Texture
  bool _initialized;         // Oggetto usabile?

  // Blocchiamo le operazioni di copia di una istanza di Cube:
  // non possiamo condividere un VAO
  Cube& operator=(const Cube& other);
  Cube(const Cube& other);
};

#endif