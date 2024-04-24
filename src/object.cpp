#include "object.h"

#include <imgui.h>

#include "logger.h"
#include "utilities.h"

IRenderableObject::~IRenderableObject() {
  //
}

Terrain::Terrain() : shader_(nullptr) {
  LOG_TRACE("Terrain()");
}

Terrain::~Terrain() {  //
  LOG_TRACE("~Terrain()");
}

void Terrain::Draw() const {
  const std::vector<Mesh>& meshes = model_.meshes();
  // LOG_INFO("This model contains {} meshes", meshes.size());

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

const Shader* Terrain::GetShader() const {
  return shader_;
}

void Terrain::SetRenderSettings() const {
  glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void Terrain::ShowSettingsGUI() {
  //
}

const Model& Terrain::model() const {
  return model_;
}

void Terrain::model(const Model& model) {
  model_ = model;
}

const std::string& Terrain::name() const {
  return name_;
}

void Terrain::name(const std::string& name) {
  name_ = name;
}

// ---

ProgressiveMesh::ProgressiveMesh() : shader_(nullptr) {
  LOG_TRACE("ProgressiveMesh()");
}

ProgressiveMesh::~ProgressiveMesh() {  //
  LOG_TRACE("~ProgressiveMesh()");
}

void ProgressiveMesh::Draw() const {}

void ProgressiveMesh::SetRenderSettings() const {}

void ProgressiveMesh::ShowSettingsGUI() {
  //
}

const Shader* ProgressiveMesh::GetShader() const {
  return shader_;
}

const Model& ProgressiveMesh::model() const {
  return model_;
}

void ProgressiveMesh::model(const Model& model) {
  model_ = model;
}

const std::string& ProgressiveMesh::name() const {
  return name_;
}

void ProgressiveMesh::name(const std::string& name) {
  name_ = name;
}

// ---

StaticModel::StaticModel(const std::string& name, const Model& model,
                         const Shader* shader)
    : name_(name),
      base_model_(model),
      shader_(shader),
      subdiv_level_(0),
      subdiv_algo_(sa::SubDiv::NONE),
      current_subdiv_level_(0),
      current_subdiv_algo_(sa::SubDiv::NONE)
// subdiv_strategy_(nullptr)
{
  LOG_TRACE("StaticModel(const Model&)");
}

StaticModel::~StaticModel() {
  // delete subdiv_strategy_;
  LOG_TRACE("~StaticModel()");
}

void StaticModel::Draw() const {
  // TODO subdiv_model_
  const std::vector<Mesh>& meshes = base_model_.meshes();
  // LOG_INFO("This model contains {} meshes", meshes.size());

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

void StaticModel::SetRenderSettings() const {
  glPatchParameteri(GL_PATCH_VERTICES, 3);
}

void StaticModel::ShowSettingsGUI() {
  ImGui::SliderInt("subdivision level", &subdiv_level_, 0, 5);

  if (ImGui::BeginCombo("subdivision algorithm",
                        sa::kSubdivisions.at(subdiv_algo_).c_str())) {
    for (const auto& [subdiv_key, subdiv_name] : sa::kSubdivisions) {
      const bool is_selected = (subdiv_algo_ == subdiv_key);
      if (ImGui::Selectable(subdiv_name.c_str(), is_selected)) {
        subdiv_algo_ = subdiv_key;
      }
    }

    ImGui::EndCombo();
  }

  ImGui::Text("Current subdiv algo is %s",
              sa::kSubdivisions.at(current_subdiv_algo_).c_str());
  ImGui::Text("Current subdiv level is %d", current_subdiv_level_);

  if (ImGui::Button("Apply Subdivision!")) {
    // TODO call to subdivision
    current_subdiv_algo_ = subdiv_algo_;
    current_subdiv_level_ = subdiv_level_;
  }

  ImGui::Spacing();
}

const Shader* StaticModel::GetShader() const {
  return shader_;
}

const Model& StaticModel::model() const {
  return base_model_;
}

void StaticModel::model(const Model& model) {
  base_model_ = model;
}

const std::string& StaticModel::name() const {
  return name_;
}

void StaticModel::name(const std::string& name) {
  name_ = name;
}
