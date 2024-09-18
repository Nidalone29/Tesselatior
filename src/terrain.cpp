#include "object.h"

Terrain::Terrain() : shader_(nullptr) {
  LOG_TRACE("Terrain()");
}

Terrain::~Terrain() {  //
  LOG_TRACE("~Terrain()");
  delete terrain_;
}

void Terrain::Draw() const {
  glBindVertexArray(terrain_->vao());

  terrain_->material().BindTextures();

  glEnableVertexAttribArray(to_underlying(ATTRIB_ID::POSITIONS));
  glEnableVertexAttribArray(to_underlying(ATTRIB_ID::NORMALS));
  glEnableVertexAttribArray(to_underlying(ATTRIB_ID::TEXTURE_COORDS));

  // clang-format off
    const Material& material = terrain_->material();
    shader_->SetUniformVec3("material_ambient_reflectivity", material.ambient_reflectivity());
    shader_->SetUniformVec3("material_diffuse_reflectivity", material.diffuse_reflectivity());
    shader_->SetUniformVec3("material_specular_reflectivity", material.specular_reflectivity());
    shader_->SetUniformFloat("material_specular_glossiness_exponent", material.shininess());
  // clang-format on

  glDrawElements(GL_PATCHES, terrain_->num_indices(), GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
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

const Transform& Terrain::transform() const {
  return transform_;
}

void Terrain::transform(const Transform& transform) {
  transform_ = transform;
}

const std::string& Terrain::name() const {
  return name_;
}

void Terrain::name(const std::string& name) {
  name_ = name;
}
