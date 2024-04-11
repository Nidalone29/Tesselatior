#version 420 core

layout (vertices = 4) out;

in VS_OUT {
  vec3 normal_;
  vec3 position_;
  vec2 textcoord_;
} tcs_in[];

out TCS_OUT {
  vec3 normal_;
  vec3 position_;
  vec2 textcoord_;
} tcs_out[];

uniform float tessellation_level;

// TODO Implement LOD
void main() {
  // Invocation zero controls tessellation levels for the entire patch
  if (gl_InvocationID == 0) {
    gl_TessLevelInner[0] = tessellation_level;
    gl_TessLevelInner[1] = tessellation_level;
    gl_TessLevelOuter[0] = tessellation_level;
    gl_TessLevelOuter[1] = tessellation_level;
    gl_TessLevelOuter[2] = tessellation_level;
    gl_TessLevelOuter[3] = tessellation_level;
  }

  // Everybody copies their input to their output
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  tcs_out[gl_InvocationID].normal_ = tcs_in[gl_InvocationID].normal_;
  tcs_out[gl_InvocationID].position_ = tcs_in[gl_InvocationID].position_;
  tcs_out[gl_InvocationID].textcoord_ = tcs_in[gl_InvocationID].textcoord_;
}
