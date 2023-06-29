#ifndef SHADERCLASS_H
#define SHADERCLASS_H

#define INVALID_UNIFORM_LOCATION 0xffffffff

#include "utilities.h"
#include <string>
#include "GL/glew.h"  // prima di freeglut

/**
 * Classe astratta per la gestione degli shader e loro parametri
 */
class ShaderClass {
 public:
  /**
   * Costruttore di default
   */
  ShaderClass();

  /**
   * Distruttore
   */
  ~ShaderClass();

  /**
   * Metodo di inizializzazione della classe. Vengono chiamati
   * automaticamente i metodi virtuali load_shaders e load_done.
   *
   * @return true se l'inizializzazione è andata a buon fine
   */
  bool init();

  /**
   * Abilita l'uso del programma degli shader nella pipeline di rendering
   */
  void enable();

 protected:
  /**
   * Metodo virtuale puro che si deve occupare del caricamento degli
   * shader.
   *
   * @return true se il caricamento è andato a buon fine
   */
  virtual bool load_shaders() = 0;

  /**
   * Metodo virtuale puro che si deve occupare delle operazioni
   * post-caricamento degli shader. Tipicamente recupera tutte le location delle
   * variabili uniform negli shader.
   *
   * @return se le operazioni post-load sono andate a buon fine
   */
  virtual bool load_done() = 0;

  /**
   * Metodo di utilità per caricare uno shader
   * @param ShaderType tipo di shader da caricare
   * @param FileName nome del file dello shader
   *
   * @return true se il caricamento è andato a buon fine
   */
  bool add_shader(GLenum ShaderType, const std::string& FileName);

  /**
   * Metodo di utilità per recuperare la location di una variabile uniform.
   *
   * @param UniformName nome della variabile
   * @return l'handle della variabile
   */
  GLint get_uniform_location(const std::string& UniformName) const;

  GLuint _program;  // Handle del programma che contiene gli shader

 private:
  Shaders _shaders;  // Vettore fi lavoro che contiene i vari shader caricati
};

#endif  // SHADERCLASS_H