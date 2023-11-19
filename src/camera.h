#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

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
   * sets the perspective projection matrix
   *
   * @param FOVDeg Angolo del field of view in gradi
   * @param width larghezza della window
   * @param height altezza della window
   * @param znear coordinata z del near plane
   * @param zfar coordinate z del far plane
   */
  void set_projection(const float FOVDeg, const float width, const float height,
                      const float znear, const float zfar);

  /**
   * Returns the current view_matrix
   * @return view_matrix
   */
  const glm::mat4& view_matrix() const;

  /**
   * Returns the current projection_matrix
   * @return projection_matrix
   */
  const glm::mat4& projection_matrix() const;

  /**
   * Resets the view the yaw to -90deg, the pitch to 0deg and the view matrix
   * (and everything that makes it up) to the initial value
   */
  void reset_view();

  const glm::vec3& position() const;
  const glm::vec3& lookAt() const;
  const glm::vec3& up() const;

  const float& speed() const;
  void set_speed(const float speed);
  const double& sensitivity() const;
  void set_sensitivity(const double sensitivity);

  void move(const CameraMovements movement, const float timestep);
  void rotate(const double newx, const double newy, const float timestep);

 private:
  /**
   * Sets the view matrix of the camera
   *
   * This is private because move() and rotate() modify the view based on user
   * input.
   *
   * @param position position of the camera
   * @param lookat point where the camera is looking at
   * @param up the vector pointing up for the camera
   */
  void set_camera(const glm::vec3& position, const glm::vec3& lookat,
                  const glm::vec3& up);

  // defining the camera
  glm::mat4 _view_matrix;        // view transofrm matrix
  glm::mat4 _projection_matrix;  // camera pojection matrix (perspective)
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
