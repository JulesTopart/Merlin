#version 450 core

#include "constants.glsl"
#include "common.glsl"

in vec4 position;
//flat in vec3 normal;
in vec4 normal;
in vec4 color;
in vec2 texCoord;

out vec4 FragColor;


void main() {
	FragColor = color;
}