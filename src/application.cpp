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
  return Instance()._renderer;
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
    Application::GetCamera()->_cam_controller->reset();
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

  // Mouse toggle
  if (key == GLFW_KEY_O && action == GLFW_PRESS) {
    if (Application::GetAppState() == VIEWPORT_FOCUS) {
      Application::SetAppState(MENU_CONTROL);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
      Application::SetAppState(VIEWPORT_FOCUS);
      // glfwSetCursorPos(window, xpos, ypos);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
  }
}

Camera* Application::GetCamera() {
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

  _rtt = new FrameBuffer(_properties.Width, _properties.Height);
  _rtt->bind();
  glClearColor(0.1F, 0.1F, 0.1F, 0.0F);

  _main_camera = new Camera();
  init();

  _rtt->unbind();
}

Application::~Application() {
  delete _main_camera;
  delete _rtt;
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

  // init camera
  _main_camera->set_camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1),
                           glm::vec3(0, 1, 0));

  _main_camera->set_perspective(30.0F, static_cast<float>(_properties.Width),
                                static_cast<float>(_properties.Height), 0.1F,
                                100);

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

  // NOTE this has to be after shaders are initialized and enabled
  _shader.setUnifromSampler("ColorTextSampler", TEXTURE_COLOR);
  _shader.setUniformMat4("Model2World", teapot_t.getMatrix());

  _shader.setUniformMat4("World2Camera", _main_camera->CP());
  // TODO move this (to the renderer init i think)
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);

  _rtt->unbind();

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(_window, true);

  std::string glsl_version = "#version 330";
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void Application::cameraControl(double& xpos, double& ypos) {
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

  Application::GetCamera()->_cam_controller->rotate(xpos, ypos, timestep);
}

void Application::run() {
  glEnable(GL_DEBUG_OUTPUT);

  double xpos, ypos;
  glfwGetCursorPos(_window, &xpos, &ypos);

  // imgui
  bool opt_fullscreen = true;
  bool opt_padding = false;
  ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
  bool open = true;
  bool* p_open = &open;

  while (!glfwWindowShouldClose(_window)) {
    _rtt->bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //  Poll for and process events
    glfwPollEvents();

    if (_app_state == VIEWPORT_FOCUS) {
      glfwGetCursorPos(_window, &xpos, &ypos);
      cameraControl(xpos, ypos);
    }

    _shader.setUniformMat4("World2Camera", _main_camera->CP());
    _shader.setUniformVec3("camera_position", _main_camera->position());

    _renderer.render(_Teapot, _shader);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // TL;DR; this demo is more complicated than what most users you would
    // normally use. If we remove all options we are showcasing, this demo
    // would become:
    //     void ShowExampleAppDockSpace()
    //     {
    //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    //     }
    // In most cases you should be able to just call DockSpaceOverViewport()
    // and ignore all the code below! In this specific demo, we are not
    // using DockSpaceOverViewport() because:
    // - (1) we allow the host window to be floating/moveable instead of
    // filling the viewport (when opt_fullscreen == false)
    // - (2) we allow the host window to have padding (when opt_padding ==
    // true)
    // - (3) we expose many flags and need a way to have them visible.
    // - (4) we have a local menu bar in the host window (vs. you could use
    // BeginMainMenuBar() + DockSpaceOverViewport()
    //      in your code, but we don't here because we allow the window to
    //      be floating)

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
    // window not dockable into, because it would be confusing to have two
    // docking targets within each others.
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
      const ImGuiViewport* viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->WorkPos);
      ImGui::SetNextWindowSize(viewport->WorkSize);
      ImGui::SetNextWindowViewport(viewport->ID);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      window_flags |= ImGuiWindowFlags_NoTitleBar |
                      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                      ImGuiWindowFlags_NoMove;
      window_flags |=
          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    } else {
      dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
    // render our background and handle the pass-thru hole, so we ask Begin() to
    // not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
      window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window
    // is collapsed). This is because we want to keep our DockSpace() active. If
    // a DockSpace() is inactive, all active windows docked into it will lose
    // their parent and become undocked. We cannot preserve the docking
    // relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in
    // limbo and never being visible.
    if (!opt_padding) {
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    }

    ImGui::Begin("DockSpace Demo", p_open, window_flags);
    if (!opt_padding) {
      ImGui::PopStyleVar();
    }

    if (opt_fullscreen) {
      ImGui::PopStyleVar(2);
    }

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0F, 0.0F), dockspace_flags);
    }

    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("Options")) {
        // Disabling fullscreen would allow the window to be moved to the front
        // of other windows, which we can't undo at the moment without finer
        // window depth/z control.
        ImGui::MenuItem("Fullscreen", nullptr, &opt_fullscreen);
        ImGui::MenuItem("Padding", nullptr, &opt_padding);
        ImGui::Separator();

        if (ImGui::MenuItem(
                "Flag: NoSplit", "",
                (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) {
          dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
        }
        if (ImGui::MenuItem(
                "Flag: NoResize", "",
                (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) {
          dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
        }
        if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "",
                            (dockspace_flags &
                             ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) {
          dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
        }
        if (ImGui::MenuItem(
                "Flag: AutoHideTabBar", "",
                (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) {
          dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
        }
        if (ImGui::MenuItem(
                "Flag: PassthruCentralNode", "",
                (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0,
                opt_fullscreen)) {
          dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
        }
        ImGui::Separator();

        if (ImGui::MenuItem("Close", nullptr, false, p_open != nullptr))
          *p_open = false;
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }

    ImGui::ShowDemoWindow();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

    ImGui::Begin("viewport");

    // scrolled and correct.
    glm::vec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    // TODO fix float to int comparison
    if (viewportPanelSize != _rtt->getSize()) {
      _rtt->resize(viewportPanelSize.x, viewportPanelSize.y);
      _main_camera->set_perspective(30.0F, viewportPanelSize.x,
                                    viewportPanelSize.y, 0.1F, 100);
    }

    // Update the ImGui image with the new texture data
    ImGui::Image(reinterpret_cast<void*>(_rtt->getTexture()),
                 ImVec2(viewportPanelSize.x, viewportPanelSize.y), ImVec2{0, 1},
                 ImVec2{1, 0});

    ImGui::End();  // "viewport"
    _rtt->unbind();
    ImGui::PopStyleVar();

    ImGui::End();  // "DockSpace Demo"

    ImGui::Render();

    // *NOTE: this line has to be before ImGui_ImplOpenGL3_RenderDrawData
    // otherwise black screen

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap front and back buffers
    glfwSwapBuffers(_window);
  }
}