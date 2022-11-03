#version 450 core

layout (location = 0) in vec3 a_Position;

uniform mat4 view;

void main()
{
	gl_Position = view * vec4(a_Position, 1.0f);
}