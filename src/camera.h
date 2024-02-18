#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

enum class CameraMovements {
  LEFT,
  RIGHT,
  FORWARD,
  BACK,
  UP,
  DOWN
};

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
  void projection_matrix(const float FOVDeg, const float width,
                         const float height, const float znear,
                         const float zfar);

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
  void ResetView();

  const glm::vec3& position() const;
  const glm::vec3& look_at() const;
  const glm::vec3& up() const;

  float speed() const;
  float* speed();  // useful for imgui
  void speed(const float speed);

  float sensitivity() const;
  float* sensitivity();
  void sensitivity(const float sensitivity);

  void Move(const CameraMovements movement, const float timestep);
  // the timestep is not needed because of how the rotation is calculated (glfw
  // mouse offset)
  void Rotate(const double newx, const double newy);

  MousePosition mouse_position_;

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
  void SetCameraView(const glm::vec3& position, const glm::vec3& lookat,
                     const glm::vec3& up);

  // defining the camera
  glm::mat4 view_matrix_;        // view transform matrix
  glm::mat4 projection_matrix_;  // camera pojection matrix (perspective)

  glm::vec3 up_;
  glm::vec3 position_;
  glm::vec3 lookat_dir_;

  // controlling the camera
  float sensitivity_;  // mouse movement speed (deg/sec)
  float pitch_deg_;
  float yaw_deg_;

  float movement_speed_;  // keyboard movement speed
};

#endif  // CAMERA_H
