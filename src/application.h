#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif  // NDEBUG

#include "renderer.h"
#include "scene.h"
#include "camera.h"
#include "shader.h"
#include "framebuffer.h"
#include "logger.h"

enum class APP_STATE {
  VIEWPORT_FOCUS,
  MENU_CONTROL
};

// The Application class is responsible for initializing and deleting all the
// scenes and shaders
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
  void Run();

 private:
  Application();
  ~Application();

  void Init();
  void CleanUp();
  void CameraControl(const double xpos, const double ypos,
                     const float delta_time);

  void DrawImGuiLayer();
  void DrawControls();
  void DrawViewport();

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

  Props properties_;
  GLFWwindow* window_;
  bool vsync_;

  // for mouse toggle
  APP_STATE app_state_;

  // rendering specific things
  Camera main_camera_;
  Renderer* renderer_;
  std::vector<Scene*> scenes_;
  int number_of_scenes_;
  int current_scene_index_;

  // shaders
  std::vector<Shader*> shaders_;
};

#endif  // APPLICATION_H
