#include "object.h"

#include <imgui.h>

#include "logger.h"
#include "utilities.h"
#include "subdiv/loop.h"

IRenderableObject::~IRenderableObject() {
  //
}

Terrain::Terrain() : shader_(nullptr) {
  LOG_TRACE("Terrain()");
}

Terrain::~Terrain() {  //
  LOG_TRACE("~Terrain()");
  delete model_;
}

void Terrain::Draw() const {
  const std::vector<TriMesh>& meshes = model_->meshes();
  // LOG_INFO("This model contains {} meshes", meshes.size());

  for (const TriMesh& mesh : meshes) {
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

const Model* Terrain::model() const {
  return model_;
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
  delete model_;
}

void ProgressiveMesh::Draw() const {}

void ProgressiveMesh::SetRenderSettings() const {}

void ProgressiveMesh::ShowSettingsGUI() {
  //
}

const Shader* ProgressiveMesh::GetShader() const {
  return shader_;
}

const Model* ProgressiveMesh::model() const {
  return model_;
}

const std::string& ProgressiveMesh::name() const {
  return name_;
}

void ProgressiveMesh::name(const std::string& name) {
  name_ = name;
}

// ---

StaticModel::StaticModel(const std::string& name, Model* model,
                         const Shader* shader)
    : name_("Static | " + name), model_(model), shader_(shader) {
  LOG_TRACE("StaticModel(const std::string&, const Model&, const Shader*)");
}

StaticModel::~StaticModel() {
  LOG_TRACE("~StaticModel()");
  delete model_;
}

void StaticModel::Draw() const {
  const std::vector<TriMesh>& meshes = model_->meshes();

  for (const TriMesh& mesh : meshes) {
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
  glPatchParameteri(GL_PATCH_VERTICES, to_underlying(model_->mesh_type()));
}

void StaticModel::ShowSettingsGUI() {
  // TODO phong alpha
  ImGui::Text("this model contains %d vertices and %d indices",
              model_->meshes()[0].num_vertices(),
              model_->meshes()[0].num_indices());
}

const Shader* StaticModel::GetShader() const {
  return shader_;
}

const Model* StaticModel::model() const {
  return model_;
}

const std::string& StaticModel::name() const {
  return name_;
}

void StaticModel::name(const std::string& name) {
  name_ = name;
}

// ---

SubDivMesh::SubDivMesh(const std::string& name, Model* model,
                       const Shader* shader)
    : name_("SubDiv | " + name),
      base_model_(model),
      shader_(shader),
      subdiv_level_(0),
      subdiv_algo_(sa::SubDiv::NONE),
      subdiv_model_(nullptr),
      subdiv_strategy_(nullptr),
      current_subdiv_level_(0),
      current_subdiv_algo_(sa::SubDiv::NONE) {
  LOG_TRACE("SubDivMesh(const std::string&, const Model&, const Shader*)");
  if (model->meshes().size() != 1) {
    // invalid mesh for subdivision
    // maybe log model mesh count and "did you intend to create a static model?"
    throw;
  }
  subdiv_model_ = new TriMesh(model->meshes()[0]);
}

SubDivMesh::~SubDivMesh() {
  LOG_TRACE("~SubDivMesh()");
  delete base_model_;
  delete subdiv_model_;
  delete subdiv_strategy_;
}

void SubDivMesh::Draw() const {
  // TODO subdiv_model_
  const TriMesh* mesh = subdiv_model_;
  // LOG_INFO("This model contains {} meshes", meshes.size());

  glBindVertexArray(mesh->vao());

  mesh->material().BindTextures();

  glEnableVertexAttribArray(to_underlying(ATTRIB_ID::POSITIONS));
  glEnableVertexAttribArray(to_underlying(ATTRIB_ID::NORMALS));
  glEnableVertexAttribArray(to_underlying(ATTRIB_ID::TEXTURE_COORDS));

  // clang-format off
  const Material& material = mesh->material();
  shader_->SetUniformVec3("material_ambient_reflectivity", material.ambient_reflectivity());
  shader_->SetUniformVec3("material_diffuse_reflectivity", material.diffuse_reflectivity());
  shader_->SetUniformVec3("material_specular_reflectivity", material.specular_reflectivity());
  shader_->SetUniformFloat("material_specular_glossiness_exponent", material.shininess());
  // clang-format on

  glDrawElements(GL_PATCHES, mesh->num_indices(), GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
}

void SubDivMesh::SetRenderSettings() const {
  glPatchParameteri(GL_PATCH_VERTICES, 3);
}

void SubDivMesh::ShowSettingsGUI() {
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
    delete subdiv_strategy_;
    delete subdiv_model_;

    current_subdiv_algo_ = subdiv_algo_;
    current_subdiv_level_ = subdiv_level_;

    switch (current_subdiv_algo_) {
      case sa::SubDiv::NONE:
        LOG_INFO("No subdiv selected");
        break;
      case sa::SubDiv::LOOP:
        subdiv_strategy_ = new LoopSubdiv();
        break;
      default:
        throw;
        break;
    }
    subdiv_model_ =
        subdiv_strategy_->subdivide(base_model_, current_subdiv_level_);
  }

  ImGui::Spacing();

  ImGui::Text("this subdivided model contains %d vertices and %d indices",
              subdiv_model_->num_vertices(), subdiv_model_->num_indices());

  ImGui::Text("this subdivided model contains %d edges",
              subdiv_model_->edges()->size());
  ImGui::Text("this subdivided model contains %d faces",
              subdiv_model_->faces()->size());
  ImGui::Spacing();
}

const Model* SubDivMesh::model() const {
  return base_model_;
}

const Shader* SubDivMesh::GetShader() const {
  return shader_;
}

const std::string& SubDivMesh::name() const {
  return name_;
}

void SubDivMesh::name(const std::string& name) {
  name_ = name;
}