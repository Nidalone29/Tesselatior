#include "renderer.h"

#include "mesh.h"
#include "shader.h"
#include <iostream>

Renderer::Renderer() : _gl_mode(GL_FILL), _render_target(1, 1) {
  std::cout << "renderer created" << std::endl;
  _render_target.bind();

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);
  // TODO there should an initial clear color that the client can set...
  glClearColor(0.1F, 0.1F, 0.1F, 1.0F);

  _render_target.unbind();
}

void Renderer::toggleWireframe() {
  _render_target.bind();

  _gl_mode = _gl_mode == GL_FILL ? GL_LINE : GL_FILL;
  glPolygonMode(GL_FRONT_AND_BACK, _gl_mode);

  _render_target.unbind();
}

void Renderer::render(Scene& scene, const Camera& camera,
                      const Shader& shader) const {
  _render_target.bind();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader.setUniformMat4("camera_view_matrix", camera.view_matrix());
  shader.setUniformMat4("camera_projection_matrix", camera.projection_matrix());

  // this is for the fragment shader
  shader.setUniformVec3("camera_position", camera.position());

  std::vector<Object>* scene_objects = &scene.getAllObjects();

  for (Object& o : *scene_objects) {
    std::vector<Mesh>* meshes = &o.getModModel().getMeshes();
    shader.setUniformMat4("Model2World",
                          o.getModel().getTransform().getMatrix());

    for (Mesh& mesh : *meshes) {
      // this load should be done when we set the scene...
      if (!mesh.isLoaded()) {
        mesh.load();
      }

      glBindVertexArray(mesh.getVAO());

      mesh.getMaterial().bind();

      glEnableVertexAttribArray(0);  // VAO
      glEnableVertexAttribArray(1);  // VBO
      glEnableVertexAttribArray(2);  // IBO

      // clang-format off
      AmbientLight ambient_light = scene.getAmbientLight();
      shader.setUniformVec3("ambient_light_color", ambient_light.getColor());
      shader.setUniformVec3("ambient_light_intensity", ambient_light.getIntensity());

      DirectionalLight directional_light = scene.getDirectionalLight();
      shader.setUniformVec3("directional_light_color", directional_light.getColor());
      shader.setUniformVec3("directional_light_intensity", directional_light.getIntensity());
      shader.setUniformVec3("directional_light_direction", directional_light.getDirection());
	  
      Material material = mesh.getMaterial();
      shader.setUniformVec3("material_ambient_reflectivity", material.getAmbientReflectivity());  
      shader.setUniformVec3("material_diffuse_reflectivity", material.getDiffuseReflectivity());	  
      shader.setUniformVec3("material_specular_reflectivity", material.getSpecularReflectivity());
      shader.setUniformFloat("material_specular_glossiness_exponent", material.getGlossinessExponent());
      // clang-format on

      glDrawElements(GL_TRIANGLES, mesh.get_num_indices(), GL_UNSIGNED_INT,
                     nullptr);

      glBindVertexArray(0);
    }
  }

  _render_target.unbind();
}

const FrameBuffer& Renderer::target() {
  return _render_target;
}

// resizeViewport maybe is a better name? idk
void Renderer::resizeTarget(const int width, const int height) {
  _render_target.resize(width, height);
}
