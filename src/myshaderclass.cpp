#include "myshaderclass.h"
#include "utilities.h"
#include "common.h"

void MyShaderClass::set_model_transform(const glm::mat4& transform) {
  glUniformMatrix4fv(_model_transform_location, 1, GL_FALSE,
                     const_cast<float*>(&transform[0][0]));
}

void MyShaderClass::set_camera_transform(const glm::mat4& transform) {
  glUniformMatrix4fv(_camera_transform_location, 1, GL_FALSE,
                     const_cast<float*>(&transform[0][0]));
}

void MyShaderClass::set_ambient_light(const AmbientLight& al) {
  glUniform3fv(_ambient_color_location, 1, const_cast<float*>(&al.color()[0]));
  glUniform1f(_ambient_intensity_location, al.intensity());
}

void MyShaderClass::set_diffusive_light(const DiffusiveLight& dl) {
  glUniform1f(_diffusive_intensity_location, dl.intensity());
}

void MyShaderClass::set_directional_light(const DirectionalLight& dl) {
  glm::vec3 direction_normalized = glm::normalize(dl.direction());
  glUniform3fv(_directional_color_location, 1,
               const_cast<float*>(&dl.color()[0]));
  glUniform3fv(_directional_direction_location, 1,
               const_cast<float*>(&direction_normalized[0]));
}

void MyShaderClass::set_specular_light(const SpecularLight& sl) {
  glUniform1f(_specular_intensity_location, sl.intensity());
  glUniform1f(_specular_shininess_location, sl.shininess());
}

void MyShaderClass::set_camera_position(const glm::vec3& pos) {
  glUniform3fv(_camera_position_location, 1, const_cast<float*>(&pos[0]));
}

bool MyShaderClass::load_shaders() {
  return add_shader(GL_VERTEX_SHADER, "shaders/14.vert") &&
         add_shader(GL_FRAGMENT_SHADER, "shaders/14.frag");
}

bool MyShaderClass::load_done() {
  _model_transform_location = get_uniform_location("Model2World");
  _camera_transform_location = get_uniform_location("World2Camera");

  _ambient_color_location = get_uniform_location("AmbientLight.color");
  _ambient_intensity_location = get_uniform_location("AmbientLight.intensity");

  _directional_color_location = get_uniform_location("DirectionalLight.color");
  _directional_direction_location =
      get_uniform_location("DirectionalLight.direction");

  _diffusive_intensity_location =
      get_uniform_location("DiffusiveLight.intensity");

  _specular_intensity_location =
      get_uniform_location("SpecularLight.intensity");
  _specular_shininess_location =
      get_uniform_location("SpecularLight.shininess");
  _camera_position_location = get_uniform_location("CameraPosition");

  // Imposto il sampler con ID 0 per leggere la texture colore
  glUniform1i(get_uniform_location("ColorTextSampler"), TEXTURE_COLOR);

  return (_model_transform_location != INVALID_UNIFORM_LOCATION) &&
         (_camera_transform_location != INVALID_UNIFORM_LOCATION) &&
         (_ambient_color_location != INVALID_UNIFORM_LOCATION) &&
         (_ambient_intensity_location != INVALID_UNIFORM_LOCATION) &&
         (_directional_color_location != INVALID_UNIFORM_LOCATION) &&
         (_directional_direction_location != INVALID_UNIFORM_LOCATION) &&
         (_diffusive_intensity_location != INVALID_UNIFORM_LOCATION) &&
         (_specular_intensity_location != INVALID_UNIFORM_LOCATION) &&
         (_specular_shininess_location != INVALID_UNIFORM_LOCATION) &&
         (_camera_position_location != INVALID_UNIFORM_LOCATION);
}
