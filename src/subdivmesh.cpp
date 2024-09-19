#include "object.h"

#include <imgui.h>

#include "logger.h"
#include "utilities.h"
#include "subdiv/loop.h"
#include "subdiv/sqrt3.h"

SubDivMesh::SubDivMesh(const std::string& name, IMesh* model,
                       const Shader* shader)
    : name_("SubDiv | " + name),
      base_model_(model),
      shader_(shader),
      subdiv_level_(0),
      subdiv_algo_(sa::SubDiv::NONE),
      subdiv_model_(nullptr),
      subdiv_strategy_(nullptr),
      current_subdiv_level_(0),
      compatible_subdivs_(model->CompatibleSubdivs()),
      shading_ui_(1),  // default smooth shading
      current_subdiv_algo_(sa::SubDiv::NONE) {
  LOG_TRACE("SubDivMesh(const std::string&, const Model&, const Shader*)");

  subdiv_model_ = model->clone();
}

SubDivMesh::~SubDivMesh() {
  LOG_TRACE("~SubDivMesh()");
  delete base_model_;
  delete subdiv_model_;
  delete subdiv_strategy_;
}

void SubDivMesh::Draw() const {
  glBindVertexArray(subdiv_model_->vao());

  subdiv_model_->material().BindTextures();

  glEnableVertexAttribArray(to_underlying(ATTRIB_ID::POSITIONS));
  glEnableVertexAttribArray(to_underlying(ATTRIB_ID::NORMALS));
  glEnableVertexAttribArray(to_underlying(ATTRIB_ID::TEXTURE_COORDS));

  // clang-format off
  const Material& material = subdiv_model_->material();
  shader_->SetUniformVec3("material_ambient_reflectivity", material.ambient_reflectivity());
  shader_->SetUniformVec3("material_diffuse_reflectivity", material.diffuse_reflectivity());
  shader_->SetUniformVec3("material_specular_reflectivity", material.specular_reflectivity());
  shader_->SetUniformFloat("material_specular_glossiness_exponent", material.shininess());
  // clang-format on

  glDrawElements(GL_PATCHES, subdiv_model_->num_indices(), GL_UNSIGNED_INT,
                 nullptr);

  glBindVertexArray(0);
}

// TODO move to mesh
void SubDivMesh::SetRenderSettings() const {
  glPatchParameteri(GL_PATCH_VERTICES, 3);
}

void SubDivMesh::ShowSettingsGUI() {
  ImGui::SliderInt("subdivision level", &subdiv_level_, 0, 5);

  if (ImGui::BeginCombo("subdivision algorithm",
                        sa::kSubdivisions.at(subdiv_algo_).c_str())) {
    for (const sa::SubDiv subdiv_key : compatible_subdivs_) {
      const bool is_selected = (subdiv_algo_ == subdiv_key);
      if (ImGui::Selectable(sa::kSubdivisions.at(subdiv_key).c_str(),
                            is_selected)) {
        subdiv_algo_ = subdiv_key;
      }
    }

    ImGui::EndCombo();
  }

  ImGui::RadioButton("Flat Shading", &shading_ui_, 0);
  ImGui::SameLine();
  ImGui::RadioButton("Smooth Shading", &shading_ui_, 1);

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
        subdiv_strategy_ = new NoneSubdiv();
        LOG_INFO("No subdiv selected");
        break;
      case sa::SubDiv::LOOP:
        subdiv_strategy_ = new LoopSubdiv();
        break;
      case sa::SubDiv::SQRT3:
        subdiv_strategy_ = new Sqrt3Subdiv();
        break;
      default:
        throw;
        break;
    }
    if (subdiv_model_ != nullptr) {
      subdiv_model_ =
          subdiv_strategy_->subdivide(base_model_, current_subdiv_level_);

      switch (shading_ui_) {
        case 1:
          subdiv_model_->GenerateOpenGLBuffersWithSmoothShading();
          break;
        case 0:
          subdiv_model_->GenerateOpenGLBufferWithFlatShading();
          break;
        default:
          throw;  // invalid for some reason
      }
    } else {
      subdiv_model_ = base_model_->clone();
    }
  }

  ImGui::Spacing();

  ImGui::Text("this subdivided model contains %d vertices and %d indices",
              subdiv_model_->num_vertices(), subdiv_model_->num_indices());

  ImGui::Text("this subdivided model contains %d edges",
              subdiv_model_->num_edges());
  ImGui::Text("this subdivided model contains %d faces",
              subdiv_model_->num_faces());
  ImGui::Spacing();
}

void SubDivMesh::ApplySmoothShading() {
  base_model_->ApplySmoothNormals();
  delete subdiv_model_;
  subdiv_model_ = base_model_->clone();
}

const Transform& SubDivMesh::transform() const {
  return transform_;
}

void SubDivMesh::transform(const Transform& transform) {
  transform_ = transform;
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