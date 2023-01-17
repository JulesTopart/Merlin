#version 330 core

layout (location = 0) out vec4 o_Color;
in vec3 color;

void main()
{
	o_Color = vec4(color,1.0);
}