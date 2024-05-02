#include "application.h"

#include <chrono>

#include <spdlog/spdlog.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/gtx/string_cast.hpp>

#include "camera.h"
#include "light.h"
#include "transform.h"
#include "matrix_math.h"
#include "scene.h"
#include "renderer.h"
#include "shader.h"
#include "logger.h"
#include "utilities.h"

using namespace std::chrono_literals;
using hr_clock = std::chrono::high_resolution_clock;
using clock_ms = std::chrono::milliseconds;

// imgui
constexpr ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
constexpr ImGuiColorEditFlags color_flags = ImGuiColorEditFlags_NoAlpha |
                                            ImGuiColorEditFlags_PickerHueBar |
                                            ImGuiColorEditFlags_DisplayRGB;
constexpr ImGuiSliderFlags slider_flags = ImGuiSliderFlags_AlwaysClamp;

constexpr ImGuiWindowFlags window_flags =
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

Renderer& Application::GetRenderer() {
  return *(Instance().renderer_);
}

APP_STATE Application::GetAppState() {
  return Instance().app_state_;
}

void Application::SetAppState(const APP_STATE to_add) {
  Instance().app_state_ = to_add;
}

// handling key inputs
void InputHandle(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  // camera reset
  if (key == GLFW_KEY_R && action == GLFW_PRESS) {
    Application::GetCamera().ResetView();
  }

  // wireframe mode
  if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
    Application::GetRenderer().ToggleWireframe();
  }

  ImGuiIO& io = ImGui::GetIO();
  // Mouse toggle
  // we only want to toggle the mouse when the window is hovered, otherwise
  // there is a "weird bug" with glfw self-centering the cursor outside a window
  // that causes the camera to jump
  if (key == GLFW_KEY_O && action == GLFW_PRESS &&
      glfwGetWindowAttrib(window, GLFW_HOVERED)) {
    if (Application::GetAppState() == APP_STATE::VIEWPORT_FOCUS) {
      Application::SetAppState(APP_STATE::MENU_CONTROL);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    } else {
      Application::SetAppState(APP_STATE::VIEWPORT_FOCUS);
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      glfwGetCursorPos(window, &Application::GetCamera().mouse_position_.xpos,
                       &Application::GetCamera().mouse_position_.ypos);
      io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }
  }
}

Camera& Application::GetCamera() {
  return Instance().main_camera_;
}

GLFWwindow* Application::GetWindow() {
  return Instance().window_;
}

Application& Application::Instance() {
  static Application instance_;
  return instance_;
}

Application::Application()
    : vsync_(true),
      app_state_(APP_STATE::VIEWPORT_FOCUS),
      current_scene_index_(0),
      selected_obj_index_(-1) {
  LOG_TRACE("Application()");
  Init();
}

void Application::CleanUp() {
  delete renderer_;

  for (int i = 0; i < shaders_.size(); i++) {
    delete shaders_[i];
  }

  for (int i = 0; i < scenes_.size(); i++) {
    delete scenes_[i];
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}

Application::~Application() {
  LOG_TRACE("~Application()");
  CleanUp();
}

void Application::Init() {
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::trace);
#endif  // NDEBUG

  // Initialize the window library
  if (!glfwInit()) {
    LOG_ERROR("GLFW Init fail");
    throw AppInitException();
  }
  LOG_INFO("Initialized GLFW");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  // Create a windowed mode window and its OpenGL context
  window_ = glfwCreateWindow(properties_.Width, properties_.Height,
                             properties_.Title.c_str(), nullptr, nullptr);
  if (!window_) {
    glfwTerminate();
    LOG_ERROR("GLFW Window creation fail");
    throw AppInitException();
  }
  // Make the window's context current
  glfwMakeContextCurrent(window_);

  // initialize everything else
  GLenum res = glewInit();
  if (res != GLEW_OK) {
    LOG_ERROR("GLEW Init fail");
    throw AppInitException();
  }
  LOG_INFO("Initialized GLEW");

  // input settings
  glfwSetKeyCallback(window_, InputHandle);

  // cursor settings
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  if (glfwRawMouseMotionSupported()) {
    LOG_INFO("Raw mouse input detected -> enabled raw mouse input");
    glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  }

  glfwSetWindowPos(window_, 100, 100);
  glfwSetCursorPos(window_, 0, 0);

  glfwSwapInterval(vsync_);

  // TODO fix with std::make_unique and smart pointers
  renderer_ = new Renderer();

  // pointers because if i copy to the vector then it's gonna destroy the local
  // Shader in this scope, calling the destructor and deleting the shader
  // program
  Shader* default_shader = new Shader();  // 3 patch
  default_shader->AddShader(GL_VERTEX_SHADER, "shaders/pass_through.vert");
  default_shader->AddShader(GL_TESS_CONTROL_SHADER,
                            "shaders/pass_through_tri.tesc");
  default_shader->AddShader(GL_TESS_EVALUATION_SHADER,
                            "shaders/phong_tessellation.tese");
  default_shader->AddShader(GL_FRAGMENT_SHADER, "shaders/phong.frag");

  default_shader->Init();
  shaders_.push_back(default_shader);

  Shader* terrain_shader = new Shader();  // 4 patch
  terrain_shader->AddShader(GL_VERTEX_SHADER, "shaders/pass_through.vert");
  terrain_shader->AddShader(GL_TESS_CONTROL_SHADER, "shaders/cameraLOD.tesc");
  terrain_shader->AddShader(GL_TESS_EVALUATION_SHADER, "shaders/terrain.tese");
  terrain_shader->AddShader(GL_FRAGMENT_SHADER, "shaders/phong.frag");

  terrain_shader->Init();
  shaders_.push_back(terrain_shader);

  /*
  // init scenes
  Scene* flower_scene = new Scene("Flower");
  Model flower_object(MESH_TYPE::TRIANGLES, "models/flower/flower.obj",
                      aiProcess_Triangulate);
  Transform flower_t;
  flower_t.translate(0.0F, -4.0F, -15.0F);
  flower_t.rotate(-90.0F, 0.0F, 0.0F);
  flower_object.transform(flower_t);
  StaticModel* sf = new StaticModel("flower", flower_object, default_shader);
  flower_scene->AddObject(sf);

  Model dragon(MESH_TYPE::TRIANGLES, "models/dragon.obj");
  StaticModel* dragon_model = new StaticModel("dragon", dragon, default_shader);
  flower_scene->AddObject(dragon_model);

  //  there is probably a more efficient way of doing this
  scenes_.push_back(flower_scene);

  Scene* teapot_scene = new Scene("Teapot");
  Model teapot_object(MESH_TYPE::TRIANGLES, "models/teapot.obj",
                      aiProcess_Triangulate);
  Transform teapot_t;
  teapot_t.translate(0.0F, -1.6F, -9.0F);
  teapot_t.rotate(0.0F, 0.0F, 0.0F);
  teapot_object.transform(teapot_t);
  StaticModel* st = new StaticModel("teapot", teapot_object, default_shader);
  teapot_scene->AddObject(st);
  // there is probably a more efficient way of doing this
  scenes_.push_back(teapot_scene);
  */

  Scene* cube_scene = new Scene("Cubes");

  // questo dovrebbe avere 8 vertici e
  Model cube_tri_model(MESH_TYPE::TRIANGLES, "models/cube/cube.obj",
                       aiProcess_JoinIdenticalVertices | aiProcess_DropNormals |
                           aiProcess_GenSmoothNormals);
  Transform cube_tri_t;
  cube_tri_t.translate(2.0F, 0.0F, 0.0F);
  cube_tri_model.transform(cube_tri_t);
  StaticModel* sf = new StaticModel("cube tri", cube_tri_model, default_shader);
  cube_scene->AddObject(sf);

  Model new_cube(MESH_TYPE::TRIANGLES, "models/cube/new_cube.obj",
                 aiProcess_JoinIdenticalVertices);
  Transform cube_tri_t2;
  cube_tri_t.translate(0.0F, 2.0F, 0.0F);
  new_cube.transform(cube_tri_t);
  StaticModel* sf34 = new StaticModel("new cube tri", new_cube, default_shader);
  cube_scene->AddObject(sf34);

  Model cube_quad_model(MESH_TYPE::QUADS, "models/cube/cube_quad.obj",
                        aiProcess_JoinIdenticalVertices);
  Transform flower_t;
  flower_t.translate(-2.0F, 0.0F, 0.0F);
  cube_quad_model.transform(flower_t);
  // it should not be terrain shader but whatever this is temp
  StaticModel* sf2 =
      new StaticModel("cube quad", cube_quad_model, terrain_shader);
  cube_scene->AddObject(sf2);

  scenes_.push_back(cube_scene);
  number_of_scenes_ = static_cast<int>(scenes_.size());

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window_, true);

  std::string glsl_version = "#version 420";
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  io.IniFilename = "imgui-layout.ini";
}

void Application::CameraControl(const double xpos, const double ypos,
                                const float delta_time) {
  const int stateA = glfwGetKey(window_, GLFW_KEY_A);
  const int stateD = glfwGetKey(window_, GLFW_KEY_D);
  const int stateS = glfwGetKey(window_, GLFW_KEY_S);
  const int stateW = glfwGetKey(window_, GLFW_KEY_W);
  const int stateSPACE = glfwGetKey(window_, GLFW_KEY_SPACE);
  const int stateLSHIFT = glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT);

  if (stateA == GLFW_PRESS) {
    main_camera_.Move(CameraMovements::LEFT, delta_time);
  }
  if (stateD == GLFW_PRESS) {
    main_camera_.Move(CameraMovements::RIGHT, delta_time);
  }
  if (stateW == GLFW_PRESS) {
    main_camera_.Move(CameraMovements::FORWARD, delta_time);
  }
  if (stateS == GLFW_PRESS) {
    main_camera_.Move(CameraMovements::BACK, delta_time);
  }
  if (stateSPACE == GLFW_PRESS) {
    main_camera_.Move(CameraMovements::UP, delta_time);
  }
  if (stateLSHIFT == GLFW_PRESS) {
    main_camera_.Move(CameraMovements::DOWN, delta_time);
  }

  main_camera_.Rotate(xpos, ypos);
}

void Application::DrawViewport() {
  // Creating the Viewport imgui window
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
  ImGui::Begin("viewport", nullptr, window_flags);

  glm::vec2 viewportPanelSize = ImGui::GetContentRegionAvail();
  if (viewportPanelSize != renderer_->target().size_vector()) {
    renderer_->ResizeTarget(viewportPanelSize.x, viewportPanelSize.y);
    main_camera_.projection_matrix(30.0F, viewportPanelSize.x,
                                   viewportPanelSize.y, 0.1F, 10000);
  }

  renderer_->Render(*scenes_[current_scene_index_], main_camera_);

  // Update the ImGui image with the new texture data
  ImGui::Image(
      reinterpret_cast<void*>(renderer_->target().color_attachment_id()),
      ImVec2(viewportPanelSize.x, viewportPanelSize.y), ImVec2{0, 1},
      ImVec2{1, 0});

  ImGui::End();          // "viewport"
  ImGui::PopStyleVar();  // window padding
}

void Application::DrawControls() {
  AmbientLight* scene_ambient = scenes_[current_scene_index_]->ambient_light();
  DirectionalLight* scene_directional =
      scenes_[current_scene_index_]->directional_light();

  ImGui::Begin("stats", nullptr, window_flags);
  ImGui::Text("dear imgui says hello! (%s) (%d)", IMGUI_VERSION,
              IMGUI_VERSION_NUM);

  ImGui::Spacing();

  ImGui::SeparatorText("Scenes");
  if (ImGui::BeginListBox(
          "Current Scene",
          ImVec2(-FLT_MIN, number_of_scenes_ * 1.05F *
                               ImGui::GetTextLineHeightWithSpacing()))) {
    for (int i = 0; i < number_of_scenes_; i++) {
      const bool is_selected = (current_scene_index_ == i);
      if (ImGui::Selectable(scenes_[i]->name().c_str(), is_selected)) {
        current_scene_index_ = i;
        selected_obj_index_ = -1;
      }
      // Set the initial focus when opening the combo (scrolling +
      // keyboard navigation focus)
      if (is_selected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndListBox();  // "Current Scene"
  }

  ImGui::Spacing();

  ImGui::SeparatorText("Objects in Scene");
  const int objnum = scenes_[current_scene_index_]->NumberOfObjects();
  for (int i = 0; i < objnum; i++) {
    if (ImGui::Selectable(
            scenes_[current_scene_index_]->objects()[i]->name().c_str(),
            selected_obj_index_ == i)) {
      selected_obj_index_ = i;
    }
  }

  ImGui::Spacing();

  // Only when an abject is selected
  if (selected_obj_index_ != -1) {
    scenes_[current_scene_index_]
        ->objects()[selected_obj_index_]
        ->ShowSettingsGUI();
  }

  ImGui::Spacing();

  ImGui::SeparatorText("Scene controls");
  if (ImGui::CollapsingHeader("Lights")) {
    ImGui::Text("Ambient Light");
    ImGui::ColorPicker3("ambient color", scene_ambient->color(), color_flags);

    ImGui::Text("Directional Light");
    ImGui::ColorPicker3("directional color", scene_directional->color(),
                        color_flags);

    ImGui::Text("Light Direction");
    ImGui::DragFloat3("light direction", scene_directional->direction(), 0.01F,
                      -1.0F, 1.0F, "%.2f", ImGuiSliderFlags_None);
  }

  if (ImGui::CollapsingHeader("Camera")) {
    const glm::vec3 cam_pos = main_camera_.position();
    ImGui::Text("Position (X: %.2f, Y: %.2f, Z: %.2f)", cam_pos.x, cam_pos.y,
                cam_pos.z);

    ImGui::Text("Camera sensitivity (0.01 -> 10)");
    ImGui::SliderFloat("sens", main_camera_.sensitivity(), 0.1F, 10.0F, "%.2f",
                       ImGuiSliderFlags_None);
    ImGui::Text("Movement speed (1 -> 100)");
    ImGui::SliderFloat("speed", main_camera_.speed(), 1.0F, 100.0F, "%.2f",
                       ImGuiSliderFlags_None);
  }

  if (ImGui::CollapsingHeader("Tessellation level")) {
    ImGui::Text("tess_level_inner0");
    ImGui::SliderInt("tess_level_inner0 (Tessell level)",
                     renderer_->tess_level(), 1, renderer_->max_tessel_level());

    ImGui::SliderFloat("Displacement height", renderer_->displacement_height(),
                       0.0F, 20.0F, "%.2f", ImGuiSliderFlags_None);

    ImGui::SliderFloat("phong tessel alpha", renderer_->phong_alpha(), 0.0F,
                       1.0F, "%.2f", ImGuiSliderFlags_None);
  }

  ImGui::Separator();
  if (ImGui::Button("Toggle Vsync")) {
    vsync_ = !vsync_;
    glfwSwapInterval(vsync_);
  }

  const ImGuiIO& io = ImGui::GetIO();
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0F / io.Framerate, io.Framerate);
  ImGui::End();
}

void Application::DrawImGuiLayer() {
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

  ImGui::ShowDemoWindow();

  DrawControls();
  DrawViewport();

  ImGui::PopStyleVar();  // frame rounding
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::Run() {
  double xpos, ypos;

  std::chrono::duration<float, std::chrono::seconds::period> delta_time{16ms};
  std::chrono::time_point<hr_clock> begin_time = hr_clock::now();

  while (!glfwWindowShouldClose(window_)) {
    // Poll for and process events
    glfwPollEvents();

    if (app_state_ == APP_STATE::VIEWPORT_FOCUS) {
      glfwGetCursorPos(window_, &xpos, &ypos);
      CameraControl(xpos, ypos, delta_time.count());
    }

    DrawImGuiLayer();

    // Swap front and back buffers
    glfwSwapBuffers(window_);

    std::chrono::time_point<hr_clock> end_time = hr_clock::now();
    delta_time = std::chrono::duration_cast<clock_ms>(end_time - begin_time);
    begin_time = end_time;
  }
}
