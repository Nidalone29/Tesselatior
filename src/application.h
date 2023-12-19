#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene.h"
#include "renderer.h"
#include "camera.h"
#include "light.h"
#include "shader.h"
#include "common.h"
#include "framebuffer.h"

class Application {
 public:
  Application(const Application& other) = delete;
  Application& operator=(const Application& other) = delete;
  Application(Application&& other) = delete;
  Application& operator=(Application&& other) = delete;

  static Application& Instance();

  static GLFWwindow* GetWindow();
  static Camera& GetCamera();
  static Renderer& GetRenderer();
  static APP_STATE GetAppState();
  static void SetAppState(const APP_STATE to_add);
  void run();

 private:
  Application();
  ~Application();
  void init();
  void cameraControl(const double xpos, const double ypos,
                     const float delta_time);

  struct Props {
    std::string Title;
    int Width;
    int Height;

    Props(std::string title = "Tesselatior", int width = 1280, int height = 720)
        : Title(title), Width(width), Height(height) {}
  };

  Props _properties;
  GLFWwindow* _window;
  bool _vsync;

  // for mouse toggle
  APP_STATE _app_state;

  // rendering specific things
  Camera _main_camera;
  Renderer* _renderer;
  std::vector<Scene> _scenes;
  int _current_scene_index;

  // shaders
  Shader _shader;
};

#endif  // APPLICATION_H