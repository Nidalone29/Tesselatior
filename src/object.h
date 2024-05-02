#ifndef OBJECT_H
#define OBJECT_H

#include "shader.h"
#include "model.h"
#include "./subdiv/subdivision.h"

class IRenderableObject {
 public:
  virtual ~IRenderableObject() = 0;
  virtual void Draw() const = 0;
  virtual const Shader* GetShader() const = 0;
  virtual void SetRenderSettings() const = 0;
  virtual void ShowSettingsGUI() = 0;

  virtual const Model& model() const = 0;
  virtual void model(const Model& model) = 0;
  virtual const std::string& name() const = 0;
  virtual void name(const std::string& name) = 0;
};

class Terrain : public IRenderableObject {
 public:
  Terrain();
  ~Terrain();

  void Draw() const override;
  const Shader* GetShader() const override;
  void SetRenderSettings() const override;
  void ShowSettingsGUI() override;

  const Model& model() const override;
  void model(const Model& model) override;
  const std::string& name() const override;
  void name(const std::string& name) override;

 private:
  std::string name_;
  Model model_;
  // Not owning
  const Shader* shader_;
};

class ProgressiveMesh : public IRenderableObject {
 public:
  ProgressiveMesh();
  ~ProgressiveMesh();

  void Draw() const override;
  const Shader* GetShader() const override;
  void SetRenderSettings() const override;
  void ShowSettingsGUI() override;

  const Model& model() const override;
  void model(const Model& model) override;
  const std::string& name() const override;
  void name(const std::string& name) override;

 private:
  std::string name_;
  Model model_;
  // Not owning
  const Shader* shader_;
};

// A Static Model composed of multiple meshes
class StaticModel : public IRenderableObject {
 public:
  StaticModel(const std::string& name, const Model& model,
              const Shader* shader);
  ~StaticModel();

  void Draw() const override;
  const Shader* GetShader() const override;
  void SetRenderSettings() const override;
  void ShowSettingsGUI() override;

  const Model& model() const override;
  void model(const Model& model) override;
  const std::string& name() const override;
  void name(const std::string& name) override;

 private:
  std::string name_;
  Model model_;
  // Not owning
  const Shader* shader_;
};

// A Model that also supports uniform subdivision (because there is only one
// mesh). Owns and deletes a SubDiv Strategy
class SubDivMesh : public IRenderableObject {
 public:
  SubDivMesh(const std::string& name, const Model& model, const Shader* shader);
  ~SubDivMesh();

  void Draw() const override;
  const Shader* GetShader() const override;
  void SetRenderSettings() const override;
  void ShowSettingsGUI() override;

  const Model& model() const override;
  void model(const Model& model) override;
  const std::string& name() const override;
  void name(const std::string& name) override;

 private:
  std::string name_;
  // To submit to the subdivision algorithm (it always re-starts from the base
  // model because it's easier)
  Model base_model_;
  // Not owning
  const Shader* shader_;
  // To be rendered
  Model subdiv_model_;
  // To be submitted to subdivision algorithm
  int subdiv_level_;
  sa::SubDiv subdiv_algo_;
  // To be shown in UI
  int current_subdiv_level_;
  sa::SubDiv current_subdiv_algo_;
  // ISubdivision* subdiv_strategy_;
};

#endif  // OBJECT_H
