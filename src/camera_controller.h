#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

// similar to minecraft
enum class CameraMovements { LEFT, RIGHT, FORWARD, BACK, UP, DOWN };

class CameraController {
 public:
  CameraController();

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

  void move(const CameraMovements movement, double timestep) const;
  void rotate(double newx, double newy, double timestep);
  void reset();
  // mouse controls
  struct MousePosition {
    double xpos, ypos;
  };

 private:
  MousePosition _mouse_position;
  MousePosition getMousePosition();
  double _sensitivity;
  float _pitch_deg;
  float _yaw_deg;

  // keyboard controls
  float _movement_speed;  // movement speed
};

#endif  // CAMERA_CONTROLLER_H
