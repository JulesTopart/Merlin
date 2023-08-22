
#version 450 core

#include "constants.glsl"
#include "common.glsl"

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;

out vec3 position;
out vec3 normal;
out vec4 color;
out vec2 texCoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

float rand(vec2 co){
    return 0.00001*abs(fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453));
}

vec3 randomColor(uint index){
	float v = index/5000.0;
	vec2 co = vec2(v, v*v);
	return vec3(0.2) + normalize(vec3(rand(co*0.8738), rand(co*0.321313), rand(0.12354*co)));
}

void main() {
	vec3 offset = (vec3(getBinCoordFromIndex(gl_InstanceID)) * binSize);
	position = vec3(model * vec4(_position + offset + boundaryMin, 1.0f));

	color.w = bins[gl_InstanceID].count/32;
	color = vec4(randomColor(gl_InstanceID), color.w);
	
	normal = _normal;

	if(bins[gl_InstanceID].count == 0) gl_Position = vec4(0);
	else gl_Position = projection * view * vec4(position, 1.0f);
}