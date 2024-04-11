#version 420 core

// Il vertex shader riceve in input gli attributi dei vertici
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textcoord;

// interface block
out VS_OUT {
  vec3 normal_;
  vec3 position_;
  vec2 textcoord_;
} vs_out;

// Vertex shader that does not compute the transformations, because they are 
// offloaded to a Tessellation Evaluation Shader
void main() {
  gl_Position = vec4(position, 1.0);

  vs_out.normal_ = normal;
  vs_out.position_ = position;
  vs_out.textcoord_ = textcoord;
}
