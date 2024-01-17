
#version 450 core

#include "common.glsl"
#include "constants.glsl"

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
	float v = index/5000.0;
	vec2 co = vec2(v, v*v);
	return vec3(0.2) + normalize(vec3(rand(co*0.8738), rand(co*0.321313), rand(0.12354*co)));
}


void main() {

	vec4 water = vec4(0.25, 0.4, 1.0, 0.8);
	vec4 foam = vec4(1, 1, 1, 0.1);

	vec3 offset = vec3(particles[gl_InstanceID].position);
	position = vec3(model * vec4((_position) + offset, 1.0f));

	ivec3 binIndexVec = ivec3(floor(float(binResolution) * particles[gl_InstanceID].position/binsWidth) / (2.0) );
	uint currentBinIndex = (binIndexVec.z * binResolution * binResolution) + (binIndexVec.y * binResolution) + binIndexVec.x;



	//color = mix(foam, water, particles[gl_InstanceID].density*300.0);
	//color = vec3(length(particles[gl_InstanceID].velocity)/100.0);
	//color = mix(foam, water, particles[gl_InstanceID].density*200.0 + bins[currentBinIndex].count/32.0);
	//color = vec4(particles[gl_InstanceID].temperature/nozzleTemperature);
	color.w = 0.5;
	color = vec4(randomColor(currentBinIndex),1.0);
	//color = randomColor(gl_InstanceID);
	
	
	normal = _normal;
	texCoord = _texCoord;
	gl_Position = projection * view * vec4(position, 1.0f);
	gl_PointSize = 100.0/gl_Position.w;
}
