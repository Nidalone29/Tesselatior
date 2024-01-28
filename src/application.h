#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif  // NDEBUG

#include "scene.h"
#include "renderer.h"
#include "camera.h"
#include "light.h"
#include "shader.h"
#include "common.h"
#include "framebuffer.h"
#include "logger.h"

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

    explicit Props(const std::string& title = "Tesselatior",
                   const int width = 1280, const int height = 720)
        : Title(title), Width(width), Height(height) {
      //
    }
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
