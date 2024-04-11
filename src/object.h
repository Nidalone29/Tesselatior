#ifndef OBJECT_H
#define OBJECT_H

#include "logger.h"
#include "shader.h"
#include "model.h"
#include "utilities.h"

class IRenderableObject {
 public:
  virtual ~IRenderableObject() = default;
  virtual void Draw() const = 0;
  virtual const Shader* GetShader() const = 0;
  virtual void SetRenderSettings() const = 0;
  virtual const Model& model() const = 0;
  virtual void model(const Model& model) = 0;
};

class Terrain : public IRenderableObject {
 public:
  Terrain() : shader_(nullptr) {
    LOG_TRACE("Terrain()");
  }

  ~Terrain() {  //
    LOG_TRACE("~Terrain()");
  }

  void Draw() const override {
    const std::vector<Mesh>& meshes = model_.meshes();
    LOG_INFO("This model contains {} meshes", meshes.size());

    for (const Mesh& mesh : meshes) {
      glBindVertexArray(mesh.vao());

      mesh.material().BindTextures();

      glEnableVertexAttribArray(to_underlying(ATTRIB_ID::POSITIONS));
      glEnableVertexAttribArray(to_underlying(ATTRIB_ID::NORMALS));
      glEnableVertexAttribArray(to_underlying(ATTRIB_ID::TEXTURE_COORDS));

      // clang-format off
      const Material& material = mesh.material();
      shader_->SetUniformVec3("material_ambient_reflectivity", material.ambient_reflectivity());
      shader_->SetUniformVec3("material_diffuse_reflectivity", material.diffuse_reflectivity());
      shader_->SetUniformVec3("material_specular_reflectivity", material.specular_reflectivity());
      shader_->SetUniformFloat("material_specular_glossiness_exponent", material.shininess());
      // clang-format on

      glDrawElements(GL_PATCHES, mesh.num_indices(), GL_UNSIGNED_INT, nullptr);

      glBindVertexArray(0);
    }
  }

  void SetRenderSettings() const override {
    glPatchParameteri(GL_PATCH_VERTICES, 4);
  }

  const Shader* GetShader() const override {
    return shader_;
  }

  const Model& model() const override {
    return model_;
  }

  void model(const Model& model) override {
    model_ = model;
  }

 private:
  Model model_;
  // Not owning
  const Shader* shader_;
};

class ProgressiveMesh : public IRenderableObject {
 public:
  ProgressiveMesh() : shader_(nullptr) {
    LOG_TRACE("ProgressiveMesh()");
  }

  ~ProgressiveMesh() {  //
    LOG_TRACE("~ProgressiveMesh()");
  }

  void Draw() const override {}

  void SetRenderSettings() const override {}

  const Shader* GetShader() const override {
    return shader_;
  }

  const Model& model() const override {
    return model_;
  }

  void model(const Model& model) override {
    model_ = model;
  }

 private:
  Model model_;
  // Not owning
  const Shader* shader_;
};

class StaticModel : public IRenderableObject {
 public:
  StaticModel(const Model& model, const Shader* shader)
      : model_(model), shader_(shader) {
    LOG_TRACE("StaticModel(const Model&)");
  }

  ~StaticModel() {  //
    LOG_TRACE("~StaticModel()");
  }

  void Draw() const override {
    const std::vector<Mesh>& meshes = model_.meshes();
    LOG_INFO("This model contains {} meshes", meshes.size());

    for (const Mesh& mesh : meshes) {
      glBindVertexArray(mesh.vao());

      mesh.material().BindTextures();

      glEnableVertexAttribArray(to_underlying(ATTRIB_ID::POSITIONS));
      glEnableVertexAttribArray(to_underlying(ATTRIB_ID::NORMALS));
      glEnableVertexAttribArray(to_underlying(ATTRIB_ID::TEXTURE_COORDS));

      // clang-format off
      const Material& material = mesh.material();
      shader_->SetUniformVec3("material_ambient_reflectivity", material.ambient_reflectivity());
      shader_->SetUniformVec3("material_diffuse_reflectivity", material.diffuse_reflectivity());
      shader_->SetUniformVec3("material_specular_reflectivity", material.specular_reflectivity());
      shader_->SetUniformFloat("material_specular_glossiness_exponent", material.shininess());
      // clang-format on

      glDrawElements(GL_PATCHES, mesh.num_indices(), GL_UNSIGNED_INT, nullptr);

      glBindVertexArray(0);
    }
  }

  void SetRenderSettings() const override {
    glPatchParameteri(GL_PATCH_VERTICES, 3);
  }

  const Shader* GetShader() const override {
    return shader_;
  }

  const Model& model() const override {
    return model_;
  }

  void model(const Model& model) override {
    model_ = model;
  }

 private:
  Model model_;
  // Not owning
  const Shader* shader_;
};

#endif  // OBJECT_H
