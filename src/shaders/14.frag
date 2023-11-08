#version 330

// Questa variabile di input ha lo stesso nome di quella nel Vertex Shader
// E' importante che i nomi siano uguali perchè solo in questo modo si ha
// il passaggio delle informazioni.
// in vec3 fragment_color; c'era gà ma non era usata??

// Vettori della normali ricevuti dal vertex shader
in vec3 fragment_normal;

// Coordinate spaziali dei punti ricervuti dal vertex shader
in vec3 fragment_position;

// Coordinate di texture dei punti ricervuti dal vertex shader
in vec2 fragment_textcoord;

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

void main()
{
  // La funzione texture ritorna un vec4. Nel codice noi rappresentiamo
  // i colori con vec3 e dobbiamo quindi estrarre solo 3 componenti.
  vec4 material_color = texture(ColorTextSampler, fragment_textcoord);
  
  vec3 view_dir = normalize(camera_position - fragment_position);

  vec3 normal = normalize(fragment_normal);
  // reflect() https://registry.khronos.org/OpenGL-Refpages/gl4/html/reflect.xhtml 
  vec3 reflect_dir = normalize(reflect(directional_light_direction, normal));

  vec3 ambient_reflection = material_color.rgb * material_ambient_reflectivity * ambient_light_intensity * ambient_light_color;
  vec3 diffuse_reflection = material_color.rgb * directional_light_intensity * material_diffuse_reflectivity * max(dot(normal, -directional_light_direction), 0.0);
  vec3 specular_reflection = material_color.rgb * material_specular_reflectivity * pow(max(dot(reflect_dir, view_dir), 0.0), material_specular_glossiness_exponent);
  vec3 final_color = ambient_reflection + ((diffuse_reflection + specular_reflection) * directional_light_color);
  
  // material_color.a is the transparency (.a ==> alpha)
  out_color = vec4(final_color, material_color.a);
}