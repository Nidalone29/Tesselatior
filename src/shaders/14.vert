#version 330

// Il vertex shader riceve in input gli attributi dei vertici
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;    
layout (location = 2) in vec2 textcoord;  

uniform mat4 Model2World;
uniform mat4 World2Camera;

// Passiamo al fragment shader le informazioni sulle normali dei vertici  
out vec3 fragment_normal;

// Passiamo al fragment shader le coordinate mondo dei vertici
out vec3 fragment_position;

// Passiamo al fragment shader le coordinate mondo dei vertici
out vec2 fragment_textcoord;

void main()
{
    gl_Position = World2Camera * Model2World * vec4(position, 1.0);

    // I vettori delle normali ricevuti in input sono passati 
    // in output al fragment shader dopo essere stati trasformati 
    // con la trasformazione trasposta inversa del modello.

    mat4 Model2WorldTI = transpose(inverse(Model2World));

    fragment_normal = (Model2WorldTI * vec4(normal,0.0)).xyz;

    fragment_position = (Model2World * vec4(position,1.0)).xyz;

    fragment_textcoord = textcoord;
}