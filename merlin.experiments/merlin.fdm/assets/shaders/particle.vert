
#version 450 core

#include "constants.glsl"
#include "common.glsl"

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec3 _color;
layout(location = 3) in vec2 _texCoord;

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
	float v = float(index)/5000.0;
	vec2 co = vec2(v, v*v);
	return vec3(0.2) + normalize(vec3(rand(co*0.8738), rand(co*0.321313), rand(0.12354*co)));
}

uniform int colorCycle;

void main() {
	vec3 offset = vec3(particles[gl_InstanceID].position*scale);
	position = vec3(model * vec4((_position) + offset, 1.0f));

	//color = vec4(randomColor(gl_InstanceID), 1);
	color = vec4(randomColor(particles[gl_InstanceID].binIndex), 1);
	//color = vec3(length(particles[gl_InstanceID].velocity)/100.0);
	color.w = particles[gl_InstanceID].temperature/nozzleTemperature;
	

	/*
	uint sampleNNPos = 208;
	uint ns = bins[sampleNNPos].count;
	uint st = bins[sampleNNPos].sum - ns; 
	
	bool test = false;

	for(uint k = st; k < st+ns; k++){
		uint j = particles[k].newIndex;
			if (j == gl_InstanceID) test = true;
	}
	if(test) color = vec4(0,255,0, 1);
	else color = vec4(0,0,0, 1);
	*/

	normal = _normal;
	texCoord = _texCoord;

	if(particles[gl_InstanceID].position == vec3(0)){
		gl_Position =  projection * view * vec4(0,0,0,1);
		gl_PointSize = 0;
	}
	else{
		gl_Position = projection * view * vec4(position, 1.0f);
		gl_PointSize = 200.0/(log(gl_Position.w)*10);
	}
}