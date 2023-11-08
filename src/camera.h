#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

// similar to minecraft
enum class CameraMovements { LEFT, RIGHT, FORWARD, BACK, UP, DOWN };
struct MousePosition {
  double xpos, ypos;
};

class Camera {
 public:
  Camera();
  ~Camera();
  Camera(const Camera& other) = delete;
  Camera& operator=(const Camera& other) = delete;
  Camera(Camera&& other) = delete;
  Camera& operator=(Camera&& other) = delete;

  /**
   * Imposta la matrice di trasformazione di camera.
   *
   * @param position posizione della camera
   * @param lookat punto dove guarda la camera
   * @param up vettore che indica l'alto della camera
   */
  void set_camera(const glm::vec3& position, const glm::vec3& lookat,
                  const glm::vec3& up);

  /**
   * Ritorna la matrice di trasformazione di camera.
   *
   * @return la matrice di trasformazione di camera.
   */
  const glm::mat4& camera() const;

  /**
   * Genera la matrice di trasformazione di camera.
   *
   * @param position posizione della camera
   * @param lookat punto dove guarda la camera aka target position
   * @param up vettore che indica l'alto della camera
   *
   * @return la matrice di trasformazione di camera
   */
  static glm::mat4 camera_setting(const glm::vec3& position,
                                  const glm::vec3& lookat, const glm::vec3& up);

  /**
   * Imposta la matrice di trasformazione di proiezione prospettica.
   *
   * @param FOVDeg Angolo del field of view in gradi
   * @param width larghezza della window
   * @param height altezza della window
   * @param znear coordinata z del near plane
   * @param zfar coordinate z del far plane
   */
  void set_perspective(float FOVDeg, float width, float height, float znear,
                       float zfar);

  /**
   * Ritorna la matrice di trasformazione prospettica.
   * @return la matrice di trasformazione prospettica
   */
  const glm::mat4& projection() const;

  /**
   * Ritorna la matrice di trasformazione di proiezione prospettica.
   *
   * @param FOVDeg Angolo del field of view in gradi
   * @param width larghezza della window
   * @param height altezza della window
   * @param znear coordinata z del near plane
   * @param zfar coordinate z del far plane
   * @return la matrice di trasformazione di proiezione prospettica
   */
  glm::mat4 perspective_projection(float FOVDeg, float width, float height,
                                   float znear, float zfar);

  /**
   * Reimposta le matrici di trasformazione all'identità
   */
  void reset();

  /**
   * Ritorna la matrice di trasformazione completa.
   * @return la matrice di trasformazione completa.
   */
  const glm::mat4& CP() const;

  /**
   * Ritorna la posizione di camera in coordinate mondo
   */
  const glm::vec3& position() const;
  const glm::vec3& lookAt() const;
  const glm::vec3& up() const;

  void HandleInput(int key);

  /**
   * Ritorna l'intensità degli spostamenti
   * @return l'intensità degli spostamenti
   */
  const float& speed() const;

  /**
   * Setta l'intensità degli spostamenti
   * @param speed l'intensità degli spostamenti
   */
  void set_speed(float speed);

  void move(const CameraMovements movement, const float timestep);
  void rotate(const double newx, const double newy, const float timestep);

  // TODO this has to be fixed, it should be private and called
  // automatically after an input
  void update();

 private:
  // defining the camera
  glm::mat4 _camera;  // matrice di trasformazione di camera

  glm::mat4 _projection;  // matrice di trasformazione di proiezione

  glm::mat4 _combined;

  glm::vec3 _up;
  glm::vec3 _position;
  glm::vec3 _lookat_dir;

  // controlling the camera
  MousePosition _mouse_position;
  double _sensitivity;  // mouse movement speed
  float _pitch_deg;
  float _yaw_deg;

  float _movement_speed;  // keyboard movement speed
};

#endif  // CAMERA_H
