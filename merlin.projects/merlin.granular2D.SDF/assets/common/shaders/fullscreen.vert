//This file was automatically generated 
//DO NOT CHANGE !
#version 330 core
out vec2 texCoord;
void main(void) 
{
	float x = float((gl_VertexID & 1) << 2);
	float y = float((gl_VertexID & 2) << 1);
	texCoord.x = x * 0.5;
	texCoord.y = y * 0.5;
	gl_Position = vec4(x - 1.0, y - 1.0, 0, 1);
}