#include "application.h"

#include <iostream>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "camera.h"
#include "light.h"
#include "transform.h"
#include "matrix_math.h"
#include "renderer.h"
#include "shader.h"

#include <glm/gtx/string_cast.hpp>

/*
void render_marius() {
  Transform modelT;
  modelT = modelT * Math::rotationMatrix(0.0F, 180.0F, 0.0F);
  modelT = modelT * Math::translationMatrix(0.0F, -1.7F, -0.8F);

  Application::Instance().GetShader().set_model_transform(modelT.getMatrix());

  Application::Instance()._marius.render();
}

void render_teapot() {
  Transform modelT;
  modelT.rotate(0.0F, 0.0F, 0.0F);
  modelT.translate(0.0F, -1.6F, -10.0F);

  Application::Instance().GetShader().set_model_transform(modelT.T());

  Application::Instance()._teapot.render();
}

void render_boot() {
  Transform modelT;
  modelT.rotate(0.0F, 0.0F, 0.0F);
  modelT.translate(0.0F, -10.0F, -70.0F);

  Application::Instance().GetShader().set_model_transform(modelT.T());

  Application::Instance()._boot.render();
}

void render_flower() {
  Transform modelT;
  modelT.rotate(-90.0F, 0.0F, 0.0F);
  modelT.translate(0.0F, -4.0F, -15.0F);

  Application::Instance().GetShader().set_model_transform(modelT.T());

  Application::Instance()._flower.render();
}

void render_dragon() {
  Transform modelT;
  modelT.rotate(0.0F, 0.0F, 0.0F);
  modelT.translate(0.0F, 0.0F, -5.0F);

  Application::Instance().GetShader().set_model_transform(modelT.T());

  Application::Instance()._dragon.render();
}

void render_skull() {
  Transform modelT;
  modelT.rotate(0.0F, 0.0F, 0.0F);
  modelT.translate(0.0F, -5.0F, -20.0F);

  Application::Instance().GetShader().set_model_transform(modelT.T());

  Application::Instance()._skull.render();
}
*/
Renderer& Application::GetRenderer() {
  return *(Instance()._renderer);
}

APP_STATE Application::GetAppState() {
  return Instance()._app_state;
}

void Application::SetAppState(const APP_STATE to_add) {
  Instance()._app_state = to_add;
}

// handling key inputs
void InputHandle(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  // camera reset
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    Application::GetCamera().reset_view();
  }

  // wireframe mode
  if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
    Application::GetRenderer().toggleWireframe();
  }

  // this doesn't work at the moment
  if (key == GLFW_KEY_T && action == GLFW_PRESS) {
    // render_teapot();
  } else if (key == GLFW_KEY_B && action == GLFW_PRESS) {
    // render_boot();
  } else if (key == GLFW_KEY_K && action == GLFW_PRESS) {
    // render_skull();
  } else if (key == GLFW_KEY_G && action == GLFW_PRESS) {
    // render_dragon();
  } else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
    // render_marius();
  } else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    // render_flower();
  }

  ImGuiIO& io = ImGui::GetIO();
  // Mouse toggle
  if (key == GLFW_KEY_O && action == GLFW_PRESS) {
    if (Application::GetAppState() == VIEWPORT_FOCUS) {
      Application::SetAppState(MENU_CONTROL);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    } else {
      Application::SetAppState(VIEWPORT_FOCUS);
      // glfwSetCursorPos(window, xpos, ypos);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }
  }
}

Camera& Application::GetCamera() {
  return Instance()._main_camera;
}

Application::Application() : _app_state(VIEWPORT_FOCUS) {
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

  // TODO fix with std::make_unique and smart pointers
  _renderer = new Renderer();

  init();
}

Application::~Application() {
  delete _renderer;

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}

GLFWwindow* Application::GetWindow() {
  return Instance()._window;
}

Application& Application::Instance() {
  static Application _instance;
  return _instance;
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

  _shader.addShader(GL_VERTEX_SHADER, "shaders/14.vert");
  _shader.addShader(GL_FRAGMENT_SHADER, "shaders/14.frag");

  _shader.init();
  _shader.enable();

  // init scenes
  // TODO init all the scenes and add the option to change them (maybe)
  // default scene is teapot
  Model teapot("models/flower/flower.obj", aiProcess_Triangulate);
  Transform teapot_t;
  teapot_t = teapot_t * Math::translationMatrix(0.0F, -4.0F, -15.0F);
  teapot_t = teapot_t * Math::rotationMatrix(-90.0F, 0.0F, 0.0F);
  // TODO this will have to be passed to the shader
  teapot.setTransform(teapot_t);

  _Teapot.addObject(Object(teapot));
  _shader.setUnifromSampler("ColorTextSampler", TEXTURE_COLOR);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(_window, true);

  std::string glsl_version = "#version 330";
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  io.IniFilename = "imgui-layout.ini";
}

void Application::cameraControl(double& xpos, double& ypos) {
  float timestep =
      1.0F / glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
  int stateA = glfwGetKey(_window, GLFW_KEY_A);
  int stateD = glfwGetKey(_window, GLFW_KEY_D);
  int stateS = glfwGetKey(_window, GLFW_KEY_S);
  int stateW = glfwGetKey(_window, GLFW_KEY_W);
  int stateSPACE = glfwGetKey(_window, GLFW_KEY_SPACE);
  int stateLSHIFT = glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT);

  if (stateA == GLFW_PRESS) {
    _main_camera.move(CameraMovements::LEFT, timestep);
  }
  if (stateD == GLFW_PRESS) {
    _main_camera.move(CameraMovements::RIGHT, timestep);
  }
  if (stateW == GLFW_PRESS) {
    _main_camera.move(CameraMovements::FORWARD, timestep);
  }
  if (stateS == GLFW_PRESS) {
    _main_camera.move(CameraMovements::BACK, timestep);
  }
  if (stateSPACE == GLFW_PRESS) {
    _main_camera.move(CameraMovements::UP, timestep);
  }
  if (stateLSHIFT == GLFW_PRESS) {
    _main_camera.move(CameraMovements::DOWN, timestep);
  }

  _main_camera.rotate(xpos, ypos, timestep);
}

void Application::run() {
  glEnable(GL_DEBUG_OUTPUT);

  double xpos, ypos;

  AmbientLight scene_ambient = _Teapot.getAmbientLight();
  DirectionalLight scene_directional = _Teapot.getDirectionalLight();

  glm::vec3 ambient = scene_ambient.getColor();
  glm::vec3 directional = scene_directional.getColor();

  float camera_sens = _main_camera.sensitivity();
  float camera_speed = _main_camera.speed();

  // imgui
  constexpr ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
  constexpr ImGuiColorEditFlags color_flags = ImGuiColorEditFlags_NoAlpha |
                                              ImGuiColorEditFlags_PickerHueBar |
                                              ImGuiColorEditFlags_DisplayRGB;

  constexpr ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

  ImGuiIO& io = ImGui::GetIO();

  while (!glfwWindowShouldClose(_window)) {
    // Poll for and process events
    glfwPollEvents();

    if (_app_state == VIEWPORT_FOCUS) {
      glfwGetCursorPos(_window, &xpos, &ypos);
      cameraControl(xpos, ypos);
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2);

    // Making the window a dosckpace
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);
    // the menu bar
    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open")) {
        }
        if (ImGui::MenuItem("Quit")) {
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    // ImGui::ShowDemoWindow();

    {
      ImGui::Begin("stats", nullptr, window_flags);
      ImGui::Text("dear imgui says hello! (%s) (%d)", IMGUI_VERSION,
                  IMGUI_VERSION_NUM);
      if (ImGui::CollapsingHeader("Lights")) {
        ImGui::Text("Ambient Light");
        ImGui::ColorPicker3("ambient color", &ambient.x, color_flags);
        scene_ambient.setColor(ambient);

        ImGui::Text("Directional Light");
        ImGui::ColorPicker3("directional color", &directional.x, color_flags);
        scene_directional.setColor(directional);

        _Teapot.setAmbientLight(scene_ambient);
        _Teapot.setDirectionalLight(scene_directional);
      }
      if (ImGui::CollapsingHeader("Camera")) {
        const glm::vec3 cam_pos = _main_camera.position();
        ImGui::Text("Position (X: %.2f, Y: %.2f, Z: %.2f)", cam_pos.x,
                    cam_pos.y, cam_pos.z);

        ImGui::Text("Camera sensitivity (0.01 -> 10)");
        ImGui::SliderFloat("sens", &camera_sens, 0.1F, 10.0F, "%.2f",
                           ImGuiSliderFlags_None);
        ImGui::Text("Movement speed (1 -> 25)");
        ImGui::SliderFloat("speed", &camera_speed, 1.0F, 25.0F, "%.2f",
                           ImGuiSliderFlags_None);

        _main_camera.set_sensitivity(camera_sens);
        _main_camera.set_speed(camera_speed);
      }

      ImGui::Separator();
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0F / io.Framerate, io.Framerate);
      ImGui::End();  // "stats"
    }

    {
      // Creating the Viewport imgui window
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
      ImGui::Begin("viewport", nullptr, window_flags);

      glm::vec2 viewportPanelSize = ImGui::GetContentRegionAvail();
      // TODO fix float to int comparison
      if (viewportPanelSize != _renderer->target().getSize()) {
        _renderer->resizeTarget(viewportPanelSize.x, viewportPanelSize.y);
        _main_camera.set_projection(30.0F, viewportPanelSize.x,
                                    viewportPanelSize.y, 0.1F, 100);
      }

      _renderer->render(_Teapot, _main_camera, _shader);

      // Update the ImGui image with the new texture data
      ImGui::Image(reinterpret_cast<void*>(_renderer->target().getTexture()),
                   ImVec2(viewportPanelSize.x, viewportPanelSize.y),
                   ImVec2{0, 1}, ImVec2{1, 0});

      ImGui::End();          // "viewport"
      ImGui::PopStyleVar();  // window padding
    }

    ImGui::PopStyleVar();  // frame rounding
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap front and back buffers
    glfwSwapBuffers(_window);
  }
}