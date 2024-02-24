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

// very basic pass-through tessellation

void main() {
  gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position + gl_TessCoord.y * gl_in[1].gl_Position + gl_TessCoord.z * gl_in[2].gl_Position);

  tes_out.normal_ = gl_TessCoord.x * tes_in[0].normal_ + gl_TessCoord.y * tes_in[1].normal_ + gl_TessCoord.z * tes_in[2].normal_;
  tes_out.position_ = gl_TessCoord.x * tes_in[0].position_ + gl_TessCoord.y * tes_in[1].position_ + gl_TessCoord.z * tes_in[2].position_;
  tes_out.textcoord_ =  gl_TessCoord.x * tes_in[0].textcoord_ + gl_TessCoord.y * tes_in[1].textcoord_ + gl_TessCoord.z * tes_in[2].textcoord_;
}
