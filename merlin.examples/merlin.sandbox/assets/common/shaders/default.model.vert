//This file was automatically generated 
//DO NOT CHANGE !
#version 330 core

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec3 _color;
layout (location = 3) in vec2 _texcoord;

out Vertex{
	vec3 position;
	vec3 normal;
	vec3 color;
	vec2 texcoord;
	vec3 viewPos;
	mat3 tangentBasis;
} vout;


uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec3 viewPos;

void main() {
	vout.position = vec3(model * vec4(_position, 1.0f));
	vout.color = _color;
	vout.normal = _normal;
	vout.texcoord = _texcoord;
	vout.viewPos = viewPos;
	vout.tangentBasis = mat3(1); //set to identity

	gl_Position = vec4(vout.position, 1.0f);
}
