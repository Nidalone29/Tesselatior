#version 420 core

layout (vertices = 3) out;

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

uniform float tli0;
uniform float tlo0;
uniform float tlo1;
uniform float tlo2;

void main() {
  // invocation zero controls tessellation levels for the entire patch
  if (gl_InvocationID == 0) {
    gl_TessLevelInner[0] = tli0;

    gl_TessLevelOuter[0] = tlo0;
    gl_TessLevelOuter[1] = tlo1;
    gl_TessLevelOuter[2] = tlo2;
  }

  // Everybody copies their input to their output
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  tcs_out[gl_InvocationID].normal_ = tcs_in[gl_InvocationID].normal_;
  tcs_out[gl_InvocationID].position_ = tcs_in[gl_InvocationID].position_;
  tcs_out[gl_InvocationID].textcoord_ = tcs_in[gl_InvocationID].textcoord_;
}
