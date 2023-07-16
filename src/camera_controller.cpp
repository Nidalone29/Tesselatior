#include "camera_controller.h"
#include "application.h"
#include <iostream>

CameraController::CameraController()
    : _yaw_deg(-90.0F),
      _pitch_deg(0.0F),
      _sensitivity(1.0F),
      _movement_speed(5.0F) {
  _mouse_position = {0, 0};
}

void CameraController::move(const CameraMovements movement,
                            double timestep) const {
  Camera* camera = Application::GetCamera();

  glm::vec3 tmp, new_position;
  // TODO narrowing conversion
  float speed = _movement_speed * timestep;
  switch (movement) {
    case CameraMovements::LEFT:
      tmp = glm::cross(camera->up(), camera->lookAt());
      tmp = glm::normalize(tmp);
      new_position = camera->position() + (tmp * speed);
      break;
    case CameraMovements::RIGHT:
      tmp = glm::cross(camera->lookAt(), camera->up());
      tmp = glm::normalize(tmp);
      new_position = camera->position() + (speed * tmp);
      break;
    case CameraMovements::FORWARD:
      new_position = camera->position() + (camera->lookAt() * speed);
      break;
    case CameraMovements::BACK:
      new_position = camera->position() - (camera->lookAt() * speed);
      break;

    // these are independent from the mouse
    case CameraMovements::UP:
      new_position = camera->position() + (glm::vec3(0.0F, 1.0F, 0.0F) * speed);
      break;
    case CameraMovements::DOWN:
      new_position = camera->position() - (glm::vec3(0.0F, 1.0F, 0.0F) * speed);
      break;
  }

  camera->set_camera(new_position, new_position + camera->lookAt(),
                     camera->up());
}

void CameraController::rotate(double newx, double newy, double timestep) {
  double speed = _sensitivity * timestep;

  Camera* camera = Application::GetCamera();
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

  camera->set_camera(camera->position(), camera->position() + direction,
                     camera->up());
}

void CameraController::reset() {
  _pitch_deg = 0;
  _yaw_deg = -90.0F;

  Application::GetCamera()->reset();
}

// it will be controlled by imgui
void CameraController::set_speed(float speed) {
  //_speed = speed;
}

const float& CameraController::speed() const {
  return _movement_speed;
}

CameraController::MousePosition CameraController::getMousePosition() {
  double xpos, ypos;
  glfwGetCursorPos(Application::GetWindow(), &xpos, &ypos);
  return MousePosition{xpos, ypos};
}
