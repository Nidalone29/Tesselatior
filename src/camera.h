#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"

/**
        Classe che incapsula diverse funzioni per calcolare le matrici di
        trasformazione di camera e prospettica.

        le funzioni statiche rotation, rotation3, translation, scaling
   permettono di calcolare e ottenere in output le matrici di trasformazioni
   richieste. Possono essere usate senza istanziare la classe ma come:
   LocalTransform::rotation(...)
*/
class Camera {
 public:
  /**
          Costruttore. Inizializza le matrici all'identità.
  */
  Camera();

  /**
          Imposta la matrice di trasformazione di camera.

          @param position posizione della camera
          @param lookat punto dove guarda la camera
          @param up vettore che indica l'alto della camera
  */
  void set_camera(const glm::vec3& position, const glm::vec3& lookat,
                  const glm::vec3& up);

  /**
          Ritorna la matrice di trasformazione di camera.

          @return la matrice di trasformazione di camera.
  */
  const glm::mat4& camera() const;

  /**
          Genera la matrice di trasformazione di camera.

          @param position posizione della camera
          @param lookat punto dove guarda la camera
          @param up vettore che indica l'alto della camera

          @return la matrice di trasformazione di camera
  */
  static glm::mat4 camera_setting(const glm::vec3& position,
                                  const glm::vec3& lookat, const glm::vec3& up);

  /**
          Imposta la matrice di trasformazione di proiezione prospettica.

          @param FOVDeg Angolo del field of view in gradi
          @param width larghezza della window
          @param height altezza della window
          @param znear coordinata z del near plane
          @param zfar coordinate z del far plane
  */
  void set_perspective(float FOVDeg, float width, float height, float znear,
                       float zfar);

  /**
          Ritorna la matrice di trasformazione prospettica.
          @return la matrice di trasformazione prospettica
  */
  const glm::mat4& projection() const;

  /**
          Ritorna la matrice di trasformazione di proiezione prospettica.

          @param FOVDeg Angolo del field of view in gradi
          @param width larghezza della window
          @param height altezza della window
          @param znear coordinata z del near plane
          @param zfar coordinate z del far plane
          @return la matrice di trasformazione di proiezione prospettica
  */
  glm::mat4 perspective_projection(float FOVDeg, float width, float height,
                                   float znear, float zfar);

  /**
          Reimposta le matrici di trasformazione all'identità
  */
  void reset();

  /**
          Ritorna la matrice di trasformazione completa.
          @return la matrice di trasformazione completa.
  */
  const glm::mat4& CP() const;

  /**
          Ritorna l'intensità degli spostamenti
          @return l'intensità degli spostamenti
  */
  const float& speed() const;

  /**
          Setta l'intensità degli spostamenti
          @param speed l'intensità degli spostamenti
  */
  void set_speed(float speed);

  /**
          Funzione che gestisce i caratteri speciali come KEY_UP, KEY_DOWN,
          KEY_RIGHT, KEY_LEFT. A seconda del tasto premuto la camera si muove
          in avanti/indietro e destra/sinistra.

          @param key codice del tasto premuto
  */
  bool onSpecialKeyboard(int key);

  /**
          Funzione che gestisce gli spostamenti del mouse.
          @param x coordinata x del mouse
          @param y coordinata y del mouse
  */
  bool onMouse(int x, int y);

  /**
          Setta la posizione iniziale del mouse (es. al centro della finestra).
          @param x coordinata x del mouse
          @param y coordinata y del mouse
  */
  void set_mouse_init_position(int x, int y);

  /**
          Blocca la posizione del mouse. Dopo uno spostamento, il mouse viene
          riportato alla posizione iniziale impostata.
          @param lock true per bloccare la posizione del mouse
  */
  void lock_mouse_position(bool lock);

  /**
          Ritorna la posizione di camera in coordinate mondo
  */
  const glm::vec3& position() const;

 private:
  glm::mat4 _camera;  ///<< matrice di trasformazione di camera

  glm::mat4 _projection;  ///<< matrice di trasformazione di proiezione

  glm::mat4 _combined;

  float _speed;
  glm::vec3 _up;
  glm::vec3 _position;
  glm::vec3 _lookat_dir;

  float _mouse_lastX;
  float _mouse_lastY;
  float _mouse_speed;
  float _pitch_deg;
  float _yaw_deg;
  bool _first_mouse_call;
  bool _lock_mouse_position;

  void update();
};

#endif