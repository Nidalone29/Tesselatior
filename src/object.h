#ifndef OBJECT_H
#define OBJECT_H

#include "shader.h"
#include "transform.h"
#include "./subdiv/subdivision.h"

class IRenderableObject {
 public:
  virtual ~IRenderableObject() = 0;
  virtual void Draw() const = 0;
  [[nodiscard]] virtual const Shader* GetShader() const = 0;
  virtual void SetRenderSettings() const = 0;
  virtual void ShowSettingsGUI() = 0;
  [[nodiscard]] virtual const Transform& transform() const = 0;
  virtual void transform(const Transform& transform) = 0;
  [[nodiscard]] virtual const std::string& name() const = 0;
  virtual void name(const std::string& name) = 0;
};

// TODO
class Terrain final : public IRenderableObject {
 public:
  Terrain();  // default 10x10m?
  explicit Terrain(int size /*, also path to heightmap */);
  Terrain(int side_a, int side_b /*, also path to heightmap */);
  ~Terrain();

  void Draw() const override;
  [[nodiscard]] const Shader* GetShader() const override;
  void SetRenderSettings() const override;
  void ShowSettingsGUI() override;
  const Transform& transform() const override;
  void transform(const Transform& transform) override;

  const std::string& name() const override;
  void name(const std::string& name) override;

 private:
  std::string name_;
  QuadMesh* terrain_;
  Transform transform_;
  std::filesystem::path model_path_;
  // Not owning
  const Shader* shader_;
};

// A Static Model composed of multiple meshes
class StaticModel final : public IRenderableObject {
 public:
  StaticModel(const std::string& name, const std::vector<IMesh*>& model,
              const Shader* shader);
  ~StaticModel();

  void Draw() const override;
  [[nodiscard]] const Shader* GetShader() const override;
  void SetRenderSettings() const override;
  void ShowSettingsGUI() override;
  [[nodiscard]] const Transform& transform() const override;
  void transform(const Transform& transform) override;

  [[nodiscard]] const std::string& name() const override;
  void name(const std::string& name) override;

 private:
  std::string name_;
  std::filesystem::path model_path_;
  std::vector<IMesh*> model_;
  Transform transform_;
  // Not owning
  const Shader* shader_;
  unsigned int total_index_;
  int total_verts_;
};

// A Model that also supports uniform subdivision (because there is only one
// mesh). Owns and deletes a SubDiv Strategy
class SubDivMesh final : public IRenderableObject {
 public:
  SubDivMesh(const std::string& name, IMesh* model, const Shader* shader);
  ~SubDivMesh();

  void Draw() const override;
  [[nodiscard]] const Shader* GetShader() const override;
  void SetRenderSettings() const override;
  void ShowSettingsGUI() override;
  void ApplySmoothShading();
  [[nodiscard]] const Transform& transform() const override;
  void transform(const Transform&) override;

  [[nodiscard]] const std::string& name() const override;
  void name(const std::string& name) override;

 private:
  std::string name_;
  std::filesystem::path model_path_;
  // To submit to the subdivision algorithm (it always re-starts from the base
  // model because it's easier)
  IMesh* base_model_;
  // Not owning
  const Shader* shader_;
  // To be rendered
  IMesh* subdiv_model_;
  Transform transform_;
  // To be submitted to subdivision algorithm
  int subdiv_level_;
  sa::SubDiv subdiv_algo_;
  // To be shown in UI
  int current_subdiv_level_;
  sa::SubDiv current_subdiv_algo_;
  ISubdivision* subdiv_strategy_;
  int shading_ui_;
};

// Unsupported for now
// https://hhoppe.com/pvdrpm.pdf
// Need to learn the method and try to adapt it to the modern rendering pipeline
// (aka Tessellation shaders or Mesh shaders)
/*
class ProgressiveMesh : public IRenderableObject {
};
*/

#endif  // OBJECT_H
