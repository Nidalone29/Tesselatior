#include "camera.h"
#include "transform.h"

#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
    : _yaw_deg(-90.0F),
      _pitch_deg(0.0F),
      _sensitivity(1.0F),
      _movement_speed(5.0F) {
  reset_view();
}

Camera::~Camera() {}

void Camera::reset_view() {
  _yaw_deg = -90.0F;
  _pitch_deg = 0.0F;
  // TODO there should an initial camera position that the client can set...
  set_camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
}

void Camera::set_camera(const glm::vec3& position, const glm::vec3& lookat,
                        const glm::vec3& up) {
  _position = position;
  _up = up;
  _lookat_dir = glm::normalize(lookat - _position);

  // creating a lookat matrix (aka our view matrix)
  // LookAt: a special type of view matrix that creates a coordinate system
  // where all coordinates are rotated and translated in such a way that the
  // user is looking at a given target from a given position.
  _view_matrix = glm::lookAt(_position, _lookat_dir + _position, _up);
}

// getter for the view matrix
const glm::mat4& Camera::view_matrix() const {
  return _view_matrix;
}

const glm::mat4& Camera::projection_matrix() const {
  return _projection_matrix;
}

void Camera::set_projection(const float FOVDeg, const float width,
                            const float height, const float znear,
                            const float zfar) {
  assert(zfar > znear);
  assert(width > 0);
  assert(height > 0);
  _projection_matrix =
      glm::perspective(glm::radians(FOVDeg), width / height, znear, zfar);
}

// getter camera position
const glm::vec3& Camera::position() const {
  return _position;
}

// getter camera vettore lookat
const glm::vec3& Camera::lookAt() const {
  return _lookat_dir;
}

// getter camera up
const glm::vec3& Camera::up() const {
  return _up;
}

void Camera::move(const CameraMovements movement, const float timestep) {
  glm::vec3 tmp, new_position;
  // TODO narrowing conversion
  const float speed = _movement_speed * timestep;
  switch (movement) {
    case CameraMovements::LEFT:
      tmp = glm::cross(_up, _lookat_dir);
      tmp = glm::normalize(tmp);
      new_position = _position + (tmp * speed);
      break;
    case CameraMovements::RIGHT:
      tmp = glm::cross(_lookat_dir, _up);
      tmp = glm::normalize(tmp);
      new_position = _position + (speed * tmp);
      break;
    case CameraMovements::FORWARD:
      new_position = _position + (_lookat_dir * speed);
      break;
    case CameraMovements::BACK:
      new_position = _position - (_lookat_dir * speed);
      break;

    // these are independent from the mouse
    case CameraMovements::UP:
      new_position = _position + (glm::vec3(0.0F, 1.0F, 0.0F) * speed);
      break;
    case CameraMovements::DOWN:
      new_position = _position - (glm::vec3(0.0F, 1.0F, 0.0F) * speed);
      break;
  }

  set_camera(new_position, new_position + _lookat_dir, _up);
}

void Camera::rotate(const double newx, const double newy,
                    const float timestep) {
  double speed = _sensitivity * timestep;

  double xoffset = newx - _mouse_position.xpos;
  double yoffset = _mouse_position.ypos - newy;

  _mouse_position.xpos = newx;
  _mouse_position.ypos = newy;

  xoffset *= speed;
  yoffset *= speed;

  // TODO narrowing conversion
  _yaw_deg += xoffset;
  _pitch_deg += yoffset;

  // for not rotating backwards indefinetly
  if (_pitch_deg > 89.0F) {
    _pitch_deg = 89.0F;
  } else if (_pitch_deg < -89.0F) {
    _pitch_deg = -89.0F;
  }

  glm::vec3 direction;
  direction.x = cos(glm::radians(_yaw_deg)) * cos(glm::radians(_pitch_deg));
  direction.y = sin(glm::radians(_pitch_deg));
  direction.z = sin(glm::radians(_yaw_deg)) * cos(glm::radians(_pitch_deg));
  direction = glm::normalize(direction);

  set_camera(_position, _position + direction, _up);
}

// it will be controlled by imgui
void Camera::set_speed(const float speed) {
  _movement_speed = speed;
}

const float& Camera::speed() const {
  return _movement_speed;
}

void Camera::set_sensitivity(const double sensitivity) {
  _sensitivity = sensitivity;
}

const double& Camera::sensitivity() const {
  return _sensitivity;
}

void Camera::set_mouseposition(const double x, const double y) {
  _mouse_position.xpos = x;
  _mouse_position.ypos = y;
}
