#include "renderer.h"

#include "mesh.h"
#include "shader.h"
#include <iostream>

Renderer::Renderer() : _gl_mode(GL_FILL) {
  std::cout << "renderer created" << std::endl;
}

void Renderer::toggleWireframe() {
  _gl_mode = (_gl_mode == GL_FILL) ? GL_LINE : GL_FILL;

  glPolygonMode(GL_FRONT_AND_BACK, _gl_mode);
}

void Renderer::render(Scene& scene, const Shader& shader) const {
  std::vector<Object>* scene_objects = &scene.getAllObjects();

  for (Object& o : *scene_objects) {
    std::vector<Mesh>* meshes = &o.getModModel().getMeshes();

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
}
