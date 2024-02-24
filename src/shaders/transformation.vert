#version 420 core

// Il vertex shader riceve in input gli attributi dei vertici
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textcoord;

uniform mat4 Model2World;

uniform mat4 camera_view_matrix;
uniform mat4 camera_projection_matrix;

// interface block
out VS_OUT {
  vec3 normal_;
  vec3 position_;
  vec2 textcoord_;
} vs_out;

void main() {
  gl_Position = camera_projection_matrix * camera_view_matrix * Model2World * vec4(position, 1.0);

  // I vettori delle normali ricevuti in input sono passati 
  // in output al fragment shader dopo essere stati trasformati 
  // con la trasformazione trasposta inversa del modello.

  mat4 Model2WorldTI = transpose(inverse(Model2World));

  vs_out.normal_ = normal;
  // this is used to compute the specular reflection
  vs_out.position_ = (Model2World * vec4(position, 1.0)).xyz;
  vs_out.textcoord_ = textcoord;
}
