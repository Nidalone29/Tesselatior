#ifndef APPLICATION_H
#define APPLICATION_H

// #include "scene.h"
// #include "renderer.h"
#include "camera.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "myshaderclass.h"

#include "teapot.h"
#include "skull.h"
#include "flower.h"
#include "boot.h"
#include "dragon.h"
#include "marius.h"

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

  void run(/*Renderer& render*/);

  // models
  Teapot _teapot;
  Skull _skull;
  Flower _flower;
  Boot _boot;
  Dragon _dragon;
  Marius _marius;

 private:
  Application();
  ~Application();
  void init();
  void cameraControl();

  struct Props {
    const char* Title;
    int Width;
    int Height;

    // this is a renderer prop tho....
    GLint glMode = GL_FILL;

    Props(const char* title = "Tesselatior", int width = 1280, int height = 720)
        : Title(title), Width(width), Height(height) {}
  };

  // std::vector<Scene> _scenes;
  Props _properties;
  GLFWwindow* _window;

  // rendering specifig things
  // camera
  Camera* _main_camera;

  // TODO still have to refactor all of this, i have to test the camera first
  // lights
  AmbientLight _ambient_light;
  DirectionalLight _directional_light;
  DiffusiveLight _diffusive_light;
  SpecularLight _specular_light;

  // shaders
  MyShaderClass _myshaders;
};

#endif  // APPLICATION_H