#version 450 core

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec3 _color;
layout (location = 3) in vec2 _texCoord;

out vec2 texCoord;
uniform mat4 view;
uniform mat4 model;

void main() {
	vec3 position = vec3(model * vec4(_position, 1.0f));
	texCoord = _texCoord;

	gl_Position = view * vec4(position, 1.0f);
}
