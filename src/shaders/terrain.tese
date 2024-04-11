#version 420 core

layout (quads, equal_spacing, ccw) in;

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

uniform sampler2D DisplacementTextSampler;
uniform float displacement_height;

void main() {
  // bilinear interpolations 
  // position
  vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
  vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
  vec4 p = mix(p2, p1, gl_TessCoord.y);
  
  // normals
  vec3 n1 = mix(tes_in[0].normal_, tes_in[1].normal_, gl_TessCoord.x);
  vec3 n2 = mix(tes_in[2].normal_, tes_in[3].normal_, gl_TessCoord.x);
  vec3 n = mix(n2, n1, gl_TessCoord.y);

  // texture coordinate across patch
  vec2 t1 = mix(tes_in[0].textcoord_, tes_in[1].textcoord_, gl_TessCoord.x);
  vec2 t2 = mix(tes_in[2].textcoord_, tes_in[3].textcoord_, gl_TessCoord.x);
  vec2 texCoord = mix(t2, t1, gl_TessCoord.y);

  float Height = texture(DisplacementTextSampler, texCoord).y * displacement_height;
  p.y += Height;

  gl_Position = camera_projection_matrix * camera_view_matrix * Model2World * p; 

  mat4 Model2WorldTI = transpose(inverse(Model2World));
  tes_out.normal_ = (Model2WorldTI * vec4(n, 0.0)).xyz;
  tes_out.position_ = (Model2World * p).xyz;
  tes_out.textcoord_ = texCoord;
}
