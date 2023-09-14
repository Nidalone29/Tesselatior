#ifndef APPLICATION_H
#define APPLICATION_H

#include "scene.h"
#include "renderer.h"
#include "camera.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "myshaderclass.h"
#include <vector>

class Application {
 public:
  Application(const Application& other) = delete;
  Application& operator=(const Application& other) = delete;
  Application(Application&& other) = delete;
  Application& operator=(Application&& other) = delete;

  static Application& Instance();

  static GLFWwindow* GetWindow();
  static Camera* GetCamera();
  static MyShaderClass& GetShader();
  static Renderer& GetRenderer();

  void run();

 private:
  Application();
  ~Application();
  void init();
  void cameraControl();

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
  Camera* _main_camera;
  Renderer _renderer;
  Scene _Teapot, _Skull, _Flower, _Boot, _Dragon, _Marius;
  Scene _current_scene;

  // TODO still have to refactor all of this
  // lights
  AmbientLight _ambient_light;
  DirectionalLight _directional_light;
  DiffusiveLight _diffusive_light;
  SpecularLight _specular_light;

  // shaders
  MyShaderClass _myshaders;
};

#endif  // APPLICATION_H