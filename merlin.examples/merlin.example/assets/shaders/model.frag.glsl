#version 330 core

in vec3 position;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

out vec4 FragColor;

void main(){
    FragColor = vec4(position, 1.0f);
}