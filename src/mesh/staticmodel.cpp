#include "object.h"

#include <imgui.h>

#include "../logger.h"
#include "../utilities.h"

StaticModel::StaticModel(const std::string& name,
                         const std::vector<IMesh*>& model, const Shader* shader)
    : name_("Static | " + name),
      model_(model),
      shader_(shader),
      total_index_(0),
      total_verts_(0) {
  LOG_TRACE(
      "StaticModel(const std::string&, const std::vector<IMesh*>&, const "
      "Shader*)");

  for (const IMesh* mesh : model_) {
    total_index_ += mesh->num_indices();
    total_verts_ += mesh->num_vertices();
  }
}

StaticModel::~StaticModel() {
  LOG_TRACE("~StaticModel()");

  for (IMesh* mesh : model_) {
    delete mesh;
  }
}

static int counter = 0;  // TODO refactor static
StaticModel* StaticModel::clone() {
  counter++;

  std::vector<IMesh*> copy;
  for (IMesh* m : model_) {
    copy.push_back(m->clone());
  }

  return new StaticModel(name_.substr(9) + " - " + std::to_string(counter),
                         copy, shader_);
}

void StaticModel::Draw() const {
  for (const IMesh* mesh : model_) {
    glBindVertexArray(mesh->vao());

    mesh->material().BindTextures();

    glEnableVertexAttribArray(to_underlying(ATTRIB_ID::POSITIONS));
    glEnableVertexAttribArray(to_underlying(ATTRIB_ID::NORMALS));
    glEnableVertexAttribArray(to_underlying(ATTRIB_ID::TEXTURE_COORDS));

    glPatchParameteri(GL_PATCH_VERTICES, mesh->PatchNumVertices());
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
}

void StaticModel::SetRenderSettings() const {}

void StaticModel::ShowSettingsGUI() {
  // TODO phong alpha
  ImGui::Text("this model contains %d vertices and %d indices", total_verts_,
              total_index_);
}

const Transform& StaticModel::transform() const {
  return transform_;
}

void StaticModel::transform(const Transform& transform) {
  transform_ = transform;
}

const Shader* StaticModel::GetShader() const {
  return shader_;
}

const std::string& StaticModel::name() const {
  return name_;
}

void StaticModel::name(const std::string& name) {
  name_ = name;
}
