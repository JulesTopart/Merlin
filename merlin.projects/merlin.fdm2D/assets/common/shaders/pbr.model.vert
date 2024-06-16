//This file was automatically generated 
//DO NOT CHANGE !
#version 440 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec2 texcoord;

layout(location=0) out Vertex {
	vec3 position;
	vec3 normal;
	vec2 texcoord;
	mat3 tangentBasis;
} vout;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {
	vout.position = vec3(model * vec4(position, 1.0));
	vout.texcoord = vec2(texcoord.x, 1.0-texcoord.y);
	vout.normal = normal;
	vout.tangentBasis = mat3(model) * mat3(tangent, bitangent, normal);
	gl_Position = projection * view * vec4(vout.position, 1.0f);
}
