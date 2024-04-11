#version 420 core

layout (triangles, equal_spacing, ccw) in;

in TCS_OUT {
  vec3 normal_;
  vec3 position_;
  vec2 textcoord_;
} tes_in[];

out TES_OUT {
  vec3 normal_;
  vec3 position_;
  vec2 textcoord_;
} tes_out;

uniform float alpha;

uniform mat4 Model2World;

uniform mat4 camera_view_matrix;
uniform mat4 camera_projection_matrix;
uniform float displacement_height;

uniform sampler2D DisplacementTextSampler;

// pi[i](q) in the original paper
vec3 t(vec3 q, int i){
  return q - (((q - gl_in[i].gl_Position.xyz) * tes_in[i].normal_) * tes_in[i].normal_);
}

// https://perso.telecom-paristech.fr/boubek/papers/PhongTessellation/PhongTessellation.pdf
void main() {
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;
  float w = gl_TessCoord.z;

  vec3 p0 = gl_in[0].gl_Position.xyz;
  vec3 p1 = gl_in[1].gl_Position.xyz;
  vec3 p2 = gl_in[2].gl_Position.xyz;
  
  vec3 p_uv = u * p0 + v * p1 + w * p2;
  vec3 p_star_uv = u * t(p_uv, 0) + v * t(p_uv, 1) + w * t(p_uv, 2);

  // p_alpha_star = ((1 - alpha) * p_uv + alpha * p_star_uv)
  vec3 position = mix(p_uv, p_star_uv, alpha);

  tes_out.normal_ = u * tes_in[0].normal_ + v * tes_in[1].normal_ + w * tes_in[2].normal_;
  tes_out.position_ = u * tes_in[0].position_ + v * tes_in[1].position_ + w * tes_in[2].position_;
  tes_out.textcoord_ =  u * tes_in[0].textcoord_ + v * tes_in[1].textcoord_ + w * tes_in[2].textcoord_;

  float Height = texture(DisplacementTextSampler, tes_out.textcoord_).y * displacement_height;
  position.y += Height;

  gl_Position = camera_projection_matrix * camera_view_matrix * Model2World * vec4(position, 1.0); 
}
