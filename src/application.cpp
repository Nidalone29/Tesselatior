#include "application.h"

#include <iostream>
#include <map>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"

#include "transform.h"

void render_marius() {
  LocalTransform modelT;
  modelT.rotate(0.0F, 180.0F, 0.0F);
  modelT.translate(0.0, -1.7F, -0.8F);

  Application::Instance().GetShader().set_model_transform(modelT.T());

  Application::Instance()._marius.render();
}

void render_teapot() {
  LocalTransform modelT;
  modelT.rotate(0.0F, 0.0F, 0.0F);
  modelT.translate(0.0F, -1.6F, -10.0F);

  Application::Instance().GetShader().set_model_transform(modelT.T());

  Application::Instance()._teapot.render();
}

void render_boot() {
  LocalTransform modelT;
  modelT.rotate(0.0F, 0.0F, 0.0F);
  modelT.translate(0.0F, -10.0F, -70.0F);

  Application::Instance().GetShader().set_model_transform(modelT.T());

  Application::Instance()._boot.render();
}

void render_flower() {
  LocalTransform modelT;
  modelT.rotate(-90.0F, 0.0F, 0.0F);
  modelT.translate(0.0F, -4.0F, -15.0F);

  Application::Instance().GetShader().set_model_transform(modelT.T());

  Application::Instance()._flower.render();
}

void render_dragon() {
  LocalTransform modelT;
  modelT.rotate(0.0F, 0.0F, 0.0F);
  modelT.translate(0.0F, 0.0F, -5.0F);

  Application::Instance().GetShader().set_model_transform(modelT.T());

  Application::Instance()._dragon.render();
}

void render_skull() {
  LocalTransform modelT;
  modelT.rotate(0.0F, 0.0F, 0.0F);
  modelT.translate(0.0F, -5.0F, -20.0F);

  Application::Instance().GetShader().set_model_transform(modelT.T());

  Application::Instance()._skull.render();
}

// handling key inputs
void InputHandle(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  CameraController* cm = Application::GetCamera()->_cam_controller;

  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    cm->reset();
  }

  // this doesn't work at the moment
  if (key == GLFW_KEY_T && action == GLFW_PRESS) {
    render_teapot();
  } else if (key == GLFW_KEY_B && action == GLFW_PRESS) {
    render_boot();
  } else if (key == GLFW_KEY_K && action == GLFW_PRESS) {
    render_skull();
  } else if (key == GLFW_KEY_G && action == GLFW_PRESS) {
    render_dragon();
  } else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
    render_marius();
  } else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    render_flower();
  }
}

Camera* Application::GetCamera() {
  return Instance()._main_camera;
}

Application::Application() {
  // Initialize the window library
  if (!glfwInit()) {
    std::cerr << "GLFW Init fail" << std::endl;
    exit(1);
  }

  // Create a windowed mode window and its OpenGL context
  _window = glfwCreateWindow(_properties.Width, _properties.Height,
                             _properties.Title, nullptr, nullptr);
  if (!_window) {
    glfwTerminate();
    std::cerr << "GLFW Window fail" << std::endl;
  }
  // Make the window's context current
  glfwMakeContextCurrent(_window);

  // initialize everything else
  GLenum res = glewInit();
  if (res != GLEW_OK) {
    std::cerr << "Error : " << glewGetErrorString(res) << std::endl;
    exit(1);
  }

  glClearColor(0.1F, 0.1F, 0.1F, 0.0F);

  _main_camera = new Camera();
  init();
}

Application::~Application() {
  delete _main_camera;
  glfwTerminate();
}

GLFWwindow* Application::GetWindow() {
  return Instance()._window;
}

Application& Application::Instance() {
  static Application _instance;
  return _instance;
}

MyShaderClass& Application::GetShader() {
  return Instance()._myshaders;
}

void Application::init() {
  // input settings
  glfwSetKeyCallback(_window, InputHandle);

  // cursor settings
  glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  if (glfwRawMouseMotionSupported()) {
    glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  }

  glfwSetWindowPos(_window, 100, 100);
  glfwSetCursorPos(_window, 0, 0);

  // app only works with vsync because of fixed timesteps
  glfwSwapInterval(1);

  // init scene things
  _main_camera->set_camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1),
                           glm::vec3(0, 1, 0));

  _main_camera->set_perspective(30.0F, static_cast<float>(_properties.Width),
                                static_cast<float>(_properties.Height), 0.1F,
                                100);

  _ambient_light = AmbientLight(glm::vec3(1, 1, 1), 0.2);
  _directional_light =
      DirectionalLight(glm::vec3(1, 1, 1), glm::vec3(0, 0, -1));  // 0.5
  _diffusive_light = DiffusiveLight(0.5);                         // 0.5
  _specular_light = SpecularLight(0.5, 30);

  _myshaders.init();
  _myshaders.enable();

  // TODO move this (to the renderer init i think)
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
}

void Application::cameraControl() {
  double timestep =
      1.0 / glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
  CameraController* cm = GetCamera()->_cam_controller;
  int stateA = glfwGetKey(_window, GLFW_KEY_A);
  int stateD = glfwGetKey(_window, GLFW_KEY_D);
  int stateS = glfwGetKey(_window, GLFW_KEY_S);
  int stateW = glfwGetKey(_window, GLFW_KEY_W);
  int stateSPACE = glfwGetKey(_window, GLFW_KEY_SPACE);
  int stateLSHIFT = glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT);

  if (stateA == GLFW_PRESS) {
    cm->move(CameraMovements::LEFT, timestep);
  }
  if (stateD == GLFW_PRESS) {
    cm->move(CameraMovements::RIGHT, timestep);
  }
  if (stateW == GLFW_PRESS) {
    cm->move(CameraMovements::FORWARD, timestep);
  }
  if (stateS == GLFW_PRESS) {
    cm->move(CameraMovements::BACK, timestep);
  }
  if (stateSPACE == GLFW_PRESS) {
    cm->move(CameraMovements::UP, timestep);
  }
  if (stateLSHIFT == GLFW_PRESS) {
    cm->move(CameraMovements::DOWN, timestep);
  }

  double xpos, ypos;
  glfwGetCursorPos(_window, &xpos, &ypos);
  Application::GetCamera()->_cam_controller->rotate(xpos, ypos, timestep);
}

void Application::run(/*Renderer& render*/) {
  // double time = 0.0;

  while (!glfwWindowShouldClose(_window)) {
    // Render here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cameraControl();

    _myshaders.set_camera_transform(_main_camera->CP());
    _myshaders.set_ambient_light(_ambient_light);
    _myshaders.set_directional_light(_directional_light);
    _myshaders.set_diffusive_light(_diffusive_light);
    _myshaders.set_specular_light(_specular_light);
    _myshaders.set_camera_position(_main_camera->position());

    render_teapot();

    // Swap front and back buffers
    glfwSwapBuffers(_window);

    // Poll for and process events
    glfwPollEvents();
  }
}