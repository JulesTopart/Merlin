//This file was automatically generated 
//DO NOT CHANGE !
#version 440 core

layout (location = 0) in vec4 _position;
layout (location = 1) in vec3 _normal;
layout (location = 2) in vec3 _color;
layout (location = 3) in vec2 _texcoord;


out VS_out{
	vec3 position;
	vec3 normal;
	vec3 color;
	vec2 texcoord;
} vout;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {
	vout.position = vec3(model * vec4(vec3(_position), 1.0f));
	vout.color = _color;
	vout.normal = normalize(_normal);
	vout.texcoord = _texcoord;


	gl_Position = projection * view * vec4(vout.position, 1.0f);
}
