#include "camera.h"

#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

#include "transform.h"
#include "logger.h"

Camera::Camera()
    : sensitivity_(1.0F),
      pitch_deg_(0.0F),
      yaw_deg_(-90.0F),
      movement_speed_(5.0F) {
  LOG_TRACE("Camera()");
  mouse_position_ = {0.0, 0.0};
  ResetView();
}

Camera::~Camera() {
  LOG_TRACE("~Camera()");
}

void Camera::ResetView() {
  yaw_deg_ = -90.0F;
  pitch_deg_ = 0.0F;
  // TODO there should an initial camera position that the client can set...
  SetCameraView(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
  LOG_INFO("Camera view reset");
}

void Camera::SetCameraView(const glm::vec3& position, const glm::vec3& lookat,
                           const glm::vec3& up) {
  position_ = position;
  up_ = up;
  lookat_dir_ = glm::normalize(lookat - position_);

  // creating a lookat matrix (aka our view matrix)
  // LookAt: a special type of view matrix that creates a coordinate system
  // where all coordinates are rotated and translated in such a way that the
  // user is looking at a given target from a given position.
  view_matrix_ = glm::lookAt(position_, lookat_dir_ + position_, up_);
}

// getter for the view matrix
const glm::mat4& Camera::view_matrix() const {
  return view_matrix_;
}

const glm::mat4& Camera::projection_matrix() const {
  return projection_matrix_;
}

void Camera::projection_matrix(const float FOVDeg, const float width,
                               const float height, const float znear,
                               const float zfar) {
  assert(zfar > znear);
  assert(width > 0);
  assert(height > 0);
  projection_matrix_ =
      glm::perspective(glm::radians(FOVDeg), width / height, znear, zfar);
}

// getter camera position
const glm::vec3& Camera::position() const {
  return position_;
}

// getter camera vettore lookat
const glm::vec3& Camera::look_at() const {
  return lookat_dir_;
}

// getter camera up
const glm::vec3& Camera::up() const {
  return up_;
}

void Camera::Move(const CameraMovements movement, const float timestep) {
  glm::vec3 tmp, new_position;
  const float camera_speed = movement_speed_ * timestep;
  switch (movement) {
    case CameraMovements::LEFT:
      tmp = glm::cross(up_, lookat_dir_);
      tmp = glm::normalize(tmp);
      new_position = position_ + (tmp * camera_speed);
      break;
    case CameraMovements::RIGHT:
      tmp = glm::cross(lookat_dir_, up_);
      tmp = glm::normalize(tmp);
      new_position = position_ + (camera_speed * tmp);
      break;
    case CameraMovements::FORWARD:
      new_position = position_ + (lookat_dir_ * camera_speed);
      break;
    case CameraMovements::BACK:
      new_position = position_ - (lookat_dir_ * camera_speed);
      break;
    // these are independent from the mouse
    case CameraMovements::UP:
      new_position = position_ + (glm::vec3(0.0F, 1.0F, 0.0F) * camera_speed);
      break;
    case CameraMovements::DOWN:
      new_position = position_ - (glm::vec3(0.0F, 1.0F, 0.0F) * camera_speed);
      break;
  }

  SetCameraView(new_position, new_position + lookat_dir_, up_);
}

void Camera::Rotate(const double newx, const double newy) {
  double xoffset = (newx - mouse_position_.xpos) * sensitivity_ / 100;
  double yoffset = (mouse_position_.ypos - newy) * sensitivity_ / 100;

  mouse_position_.xpos = newx;
  mouse_position_.ypos = newy;

  yaw_deg_ += static_cast<float>(xoffset);
  pitch_deg_ += static_cast<float>(yoffset);

  // for not rotating backwards indefinitely
  if (pitch_deg_ > 89.0F) {
    pitch_deg_ = 89.0F;
  } else if (pitch_deg_ < -89.0F) {
    pitch_deg_ = -89.0F;
  }

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw_deg_)) * cos(glm::radians(pitch_deg_));
  direction.y = sin(glm::radians(pitch_deg_));
  direction.z = sin(glm::radians(yaw_deg_)) * cos(glm::radians(pitch_deg_));
  direction = glm::normalize(direction);

  SetCameraView(position_, position_ + direction, up_);
}

float Camera::speed() const {
  return movement_speed_;
}

float* Camera::speed() {
  return &movement_speed_;
}

void Camera::speed(const float speed) {
  movement_speed_ = speed;
}

float Camera::sensitivity() const {
  return sensitivity_;
}

float* Camera::sensitivity() {
  return &sensitivity_;
}

void Camera::sensitivity(const float sensitivity) {
  sensitivity_ = sensitivity;
}
