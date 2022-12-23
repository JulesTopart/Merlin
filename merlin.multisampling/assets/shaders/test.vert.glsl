#version 450 core

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;

uniform mat4 view;

out vec3 normal;

void main()
{
	normal	= _normal;
	gl_Position = view * vec4(_position, 1.0f);
}