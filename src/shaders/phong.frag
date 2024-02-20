#version 420 core

in TES_OUT {
  vec3 normal_;
  vec3 position_;
  vec2 textcoord_;
} fs_in;

vec3 fragment_normal = fs_in.normal_;
vec3 fragment_position = fs_in.position_;
vec2 fragment_textcoord = fs_in.textcoord_;

// Informazioni di luce ambientale 
uniform vec3 ambient_light_color;
uniform vec3 ambient_light_intensity;

// Informazioni di luce direzionale 
uniform vec3 directional_light_color;
uniform vec3 directional_light_intensity;
uniform vec3 directional_light_direction;

uniform vec3 material_ambient_reflectivity;
uniform vec3 material_diffuse_reflectivity;
uniform vec3 material_specular_reflectivity;
uniform float material_specular_glossiness_exponent;

// Posizione della camera in coordinate mondo
uniform vec3 camera_position;

uniform sampler2D ColorTextSampler;

layout(location = 0) out vec4 out_color;

void main() {
  vec4 material_color = texture(ColorTextSampler, fragment_textcoord);

  // NOTE: material_diffuse_reflectivity is basically the base color of the material
  // if it exists and gets multiplied by the texture color there are no issues, because
  // the default texture color is 1, and 1 is the neutral element of moltiplication

  vec3 view_dir = normalize(camera_position - fragment_position);
  vec3 normal = normalize(fragment_normal);
  // reflect() https://registry.khronos.org/OpenGL-Refpages/gl4/html/reflect.xhtml 
  vec3 reflect_dir = normalize(reflect(directional_light_direction, normal));

  vec3 specular_reflection = vec3(0.0, 0.0, 0.0);
  float cosAlpha = dot(view_dir, reflect_dir);
  if (cosAlpha > 0) {
    specular_reflection = material_color.rgb * material_specular_reflectivity * pow(cosAlpha, material_specular_glossiness_exponent);
  }
  vec3 ambient_reflection = material_color.rgb * material_diffuse_reflectivity * material_ambient_reflectivity * ambient_light_intensity * ambient_light_color;
  vec3 diffuse_reflection = material_color.rgb * material_diffuse_reflectivity * directional_light_intensity * max(dot(normal, -directional_light_direction), 0.0);
  vec3 final_color = ambient_reflection + ((diffuse_reflection + specular_reflection) * directional_light_color);

  // material_color.a is the transparency (.a ==> alpha)
  out_color = vec4(final_color, material_color.a);
}
