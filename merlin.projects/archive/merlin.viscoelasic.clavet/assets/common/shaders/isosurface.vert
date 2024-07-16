#version 430

layout(location = 0) in vec4 _position;
layout(location = 1) in vec4 _normal;

out VS_OUT
{
	vec3 position;
	flat vec3 normal; // Draw with flat shading
} vs_out;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {

	gl_Position = projection * view * model * _position;
	vs_out.position = _position.xyz;
	vs_out.normal = normalize(_normal.xyz);
}