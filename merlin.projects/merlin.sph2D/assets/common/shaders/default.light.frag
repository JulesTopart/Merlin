//This file was automatically generated 
//DO NOT CHANGE !
#version 420 core

in vec3 position;
out vec4 FragColor;

//Materials data   
uniform vec3 light_color;

void main() {
    FragColor = vec4(light_color, 1.0);
}