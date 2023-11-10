#ifndef APPLICATION_H
#define APPLICATION_H

#include "scene.h"
#include "renderer.h"
#include "camera.h"
#include "light.h"
#include "shader.h"
#include "common.h"
#include "framebuffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>

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
  void cameraControl(double& xpos, double& ypos);

  struct Props {
    const char* Title;
    int Width;
    int Height;

    Props(const char* title = "Tesselatior", int width = 1280, int height = 720)
        : Title(title), Width(width), Height(height) {}
  };

  Props _properties;
  GLFWwindow* _window;

  // rendering specific things
  Camera _main_camera;
  Renderer* _renderer;
  Scene _Teapot, _Skull, _Flower, _Boot, _Dragon, _Marius;

  // shaders
  Shader _shader;

  // for mouse toggle
  APP_STATE _app_state;
};

#endif  // APPLICATION_H