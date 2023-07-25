#include "camera.h"
#include "transform.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() {
  _cam_controller = new CameraController();
  reset();
}

Camera::~Camera() {
  delete _cam_controller;
}

// la camera viene reimpostata alla posizione iniziale
// solo il cameracontroller può chiamare questa funzione
void Camera::reset() {
  //_combined = _projection = _camera = glm::mat4(1.0F);
  set_camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
  // TODO FIX
  set_perspective(30.0F, 1280, 720, 0.1F, 100);
  _position = glm::vec3(0, 0, 0);
  _lookat_dir = glm::vec3(0, 0, -1);

  _up = glm::vec3(0, 1, 0);
}

// da chiamare after control event (dove viene modificato camera(view matrix) e
// poi moltiplicata con la sua proiezione per ottenere la nuova camera)
void Camera::update() {
  _combined = _projection * _camera;
}

// da rinominare... ritorna la matrice completa
const glm::mat4& Camera::CP() const {
  return _combined;
}

// impostare la camera
void Camera::set_camera(const glm::vec3& position, const glm::vec3& lookat,
                        const glm::vec3& up) {
  _position = position;
  _up = up;
  _lookat_dir = glm::normalize(lookat - _position);
  _camera = camera_setting(_position, _lookat_dir + _position, _up);

  update();
}

// creare una matrice lookat (che sarebbe la nostra view matrix)
// LookAt: a special type of view matrix that creates a coordinate system where
// all coordinates are rotated and translated in such a way that the user is
// looking at a given target from a given position.
glm::mat4 Camera::camera_setting(const glm::vec3& position,
                                 const glm::vec3& lookat, const glm::vec3& up) {
  glm::mat4 V = glm::lookAt(position, lookat, up);
  return V;
}

// return della matrice di lookat
const glm::mat4& Camera::camera() const {
  return _camera;
}

void Camera::set_perspective(float FOVDeg, float width, float height,
                             float znear, float zfar) {
  _projection = perspective_projection(FOVDeg, width, height, znear, zfar);
  update();
}

// calcolo e riporto la matrice di proiezione prospettica in base a dei dati
// ad esempio (FoV...)
glm::mat4 Camera::perspective_projection(float FOVDeg, float width,
                                         float height, float znear,
                                         float zfar) {
  assert(zfar > znear);
  assert(width > 0);
  assert(height > 0);

  glm::mat4 p =
      glm::perspective(glm::radians(FOVDeg), width / height, znear, zfar);

  return p;
}

// ritorna la matrice di proiezione (in questo caso prospettica, ma andrebbe
// generalizzato quando implemento più camere con diverse proiezioni per
// NENGINE...)
const glm::mat4& Camera::projection() const {
  return _projection;
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