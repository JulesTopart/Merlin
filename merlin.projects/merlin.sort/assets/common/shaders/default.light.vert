//This file was automatically generated 
//DO NOT CHANGE !
#version 330 core

layout (location = 0) in vec3 _position;

out vec3 position;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {
	position = vec3(model * vec4(_position, 1.0f));
	gl_Position = projection * view * vec4(position, 1.0f);
}
