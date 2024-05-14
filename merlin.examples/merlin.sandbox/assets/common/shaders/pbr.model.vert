//This file was automatically generated 
//DO NOT CHANGE !
#version 450 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec2 texcoord;

layout(std140, binding=0) uniform TransformUniforms {
	mat4 view;
	mat4 projection;
	mat4 model;
};


layout(location=0) out Vertex {
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
} vout;


void main() {
	vout.position = vec3(model * vec4(position, 1.0));
	vout.texcoord = vec2(texcoord.x, 1.0-texcoord.y);
	vout.tangentBasis = mat3(model) * mat3(tangent, bitangent, normal);
	gl_Position = projection * view * model *  vec4(position, 1.0f);
}
