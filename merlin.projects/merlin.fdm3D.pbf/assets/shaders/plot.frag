//This file was automatically generated 
//DO NOT CHANGE !
#version 420 core

uniform sampler2D texUnit;

in VS_out{
	vec3 position;
	vec3 normal;
	vec3 color;
	vec2 texcoord;
} vin;

out vec4 FragColor;

void main() {
    FragColor = texture(texUnit, vin.texcoord);
}