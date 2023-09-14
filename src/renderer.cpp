#include "renderer.h"

#include "mesh.h"
#include <iostream>

Renderer::Renderer() : _gl_mode(GL_FILL) {
  std::cout << "renderer created" << std::endl;
}

void Renderer::toggleWireframe() {
  _gl_mode = (_gl_mode == GL_FILL) ? GL_LINE : GL_FILL;

  glPolygonMode(GL_FRONT_AND_BACK, _gl_mode);
}

void Renderer::render(Scene& scene) const {
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

      glDrawElements(GL_TRIANGLES, mesh.get_num_indices(), GL_UNSIGNED_INT,
                     nullptr);

      glBindVertexArray(0);
    }
  }
}
