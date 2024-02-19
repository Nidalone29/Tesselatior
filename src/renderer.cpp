#include "renderer.h"

#include <iostream>

#include <glm/gtx/string_cast.hpp>

#include "mesh.h"
#include "shader.h"
#include "logger.h"
#include "utilities.h"

Renderer::Renderer() : gl_mode_(GL_FILL), render_target_(1, 1) {
  LOG_TRACE("Renderer()");

  render_target_.Bind();

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.1F, 0.1F, 0.1F, 1.0F);

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

void Renderer::Render(const Scene& scene, const Camera& camera,
                      const Shader& shader) const {
  shader.Enable();
  render_target_.Bind();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader.SetUniformMat4("camera_view_matrix", camera.view_matrix());
  shader.SetUniformMat4("camera_projection_matrix", camera.projection_matrix());

  // this is for the fragment shader
  shader.SetUniformVec3("camera_position", camera.position());

  for (const Object& o : scene.objects()) {
    shader.SetUniformMat4("Model2World", o.model().transform().matrix());

    const std::vector<Mesh>& meshes = o.model().meshes();
    for (const Mesh& mesh : meshes) {
      glBindVertexArray(mesh.vao());

      mesh.material().BindTextures();

      glEnableVertexAttribArray(to_underlying(ATTRIB_ID::POSITIONS));
      glEnableVertexAttribArray(to_underlying(ATTRIB_ID::NORMALS));
      glEnableVertexAttribArray(to_underlying(ATTRIB_ID::COLOR_TEXTURE_COORDS));

      // clang-format off
      const AmbientLight& ambient_light = scene.ambient_light();
      shader.SetUniformVec3("ambient_light_color", ambient_light.color());
      shader.SetUniformVec3("ambient_light_intensity", ambient_light.intensity());

      const DirectionalLight& directional_light = scene.directional_light();
      shader.SetUniformVec3("directional_light_color", directional_light.color());
      shader.SetUniformVec3("directional_light_intensity", directional_light.intensity());
      shader.SetUniformVec3("directional_light_direction", glm::normalize(directional_light.direction()));
	  
      const Material& material = mesh.material();
      shader.SetUniformVec3("material_ambient_reflectivity", material.ambient_reflectivity());
      shader.SetUniformVec3("material_diffuse_reflectivity", material.diffuse_reflectivity());
      shader.SetUniformVec3("material_specular_reflectivity", material.specular_reflectivity());
      shader.SetUniformFloat("material_specular_glossiness_exponent", material.shininess());
      // clang-format on

      glDrawElements(GL_TRIANGLES, mesh.num_indices(), GL_UNSIGNED_INT,
                     nullptr);

      glBindVertexArray(0);
    }
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
