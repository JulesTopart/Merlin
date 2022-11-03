#version 450 core

layout (location = 0) out vec4 o_Color;

uniform vec4 color;

void main()
{
	o_Color = color;
}