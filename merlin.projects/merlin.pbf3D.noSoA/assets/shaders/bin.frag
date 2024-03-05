#version 450 core

in vec3 position;
//flat in vec3 normal;
in vec3 normal;
in vec4 color;
in vec2 texCoord;
out vec4 FragColor;

void main() {
	FragColor = color;
}