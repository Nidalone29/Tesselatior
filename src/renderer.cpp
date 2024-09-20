#include "renderer.h"

#include <iostream>

#include <glm/gtx/string_cast.hpp>

#include "./mesh/mesh.h"
#include "shader.h"
#include "logger.h"
#include "utilities.h"

Renderer::Renderer()
    : gl_mode_(GL_FILL),
      render_target_(1, 1),
      tess_level_(1),
      displacement_height_(0.0F) {
  LOG_TRACE("Renderer()");

  render_target_.Bind();

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.1F, 0.1F, 0.1F, 1.0F);

  glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &max_tessel_level_);

  render_target_.Unbind();
}

Renderer::~Renderer() {
  LOG_TRACE("~Renderer()");
}

void Renderer::SetClearColor(const float r, const float g, const float b) {
  glClearColor(r, g, b, 1.0F);
}

void Renderer::ToggleWireframe() {
  render_target_.Bind();

  gl_mode_ = (gl_mode_ == GL_FILL) ? GL_LINE : GL_FILL;
  glPolygonMode(GL_FRONT_AND_BACK, gl_mode_);
  LOG_INFO("Toggled wireframe {}", (gl_mode_ == GL_FILL) ? "OFF" : "ON");

  render_target_.Unbind();
}

void Renderer::Render(const Scene& scene, const Camera& camera) const {
  render_target_.Bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for (const IRenderableObject* o : scene.objects()) {
    const Shader* shader = o->GetShader();
    shader->Enable();
    // clang-format off
    shader->SetUniformMat4("camera_view_matrix", camera.view_matrix());
    shader->SetUniformMat4("camera_projection_matrix", camera.projection_matrix());

    // this is for the fragment shader
    shader->SetUniformVec3("camera_position", camera.position());

    shader->SetUnifromSampler("ColorTextSampler", TEXTURE_TYPE::DIFFUSE);
    shader->SetUnifromSampler("DisplacementTextSampler", TEXTURE_TYPE::DISPLACEMENT);

    shader->SetUniformFloat("alpha", alpha_);
    shader->SetUniformFloat("tessellation_level", static_cast<float>(tess_level_));
    shader->SetUniformFloat("displacement_height", displacement_height_);

    shader->SetUniformMat4("Model2World", o->transform().matrix());

 
    const AmbientLight& ambient_light = scene.ambient_light();
    shader->SetUniformVec3("ambient_light_color", ambient_light.color());
    shader->SetUniformVec3("ambient_light_intensity", ambient_light.intensity());

    const DirectionalLight& directional_light = scene.directional_light();
    shader->SetUniformVec3("directional_light_color", directional_light.color());
    shader->SetUniformVec3("directional_light_intensity", directional_light.intensity());
    shader->SetUniformVec3("directional_light_direction", glm::normalize(directional_light.direction()));
    // clang-format on

    o->Draw();
  }

  render_target_.Unbind();
}

const FrameBuffer& Renderer::target() {
  return render_target_;
}

void Renderer::ResizeTarget(const int width, const int height) {
  render_target_.Resize(width, height);
}

void Renderer::ResizeTarget(const float width, const float height) {
  render_target_.Resize(static_cast<int>(width), static_cast<int>(height));
}

int* Renderer::tess_level() {
  return &tess_level_;
}

int Renderer::max_tessel_level() const {
  return max_tessel_level_;
}

float* Renderer::displacement_height() {
  return &displacement_height_;
}

float* Renderer::phong_alpha() {
  return &alpha_;
}