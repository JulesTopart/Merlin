#version 330 core

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec3 _color;

flat out vec4 color;
uniform mat4 view;
uniform mat4 model;

void main() {
	vec3 curPos = vec3(model * vec4(_position, 1.0f));
	color = vec4(_color, 1.0f);
	gl_Position = view * vec4(curPos, 1.0f);
}
