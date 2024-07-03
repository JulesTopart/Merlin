#version 430

#include "common/uniforms.comp"
#include "common/constants.comp"
#include "common/buffers.comp"
#include "common/colors.comp"

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;

out vec3 position;
out vec3 normal;
out vec4 color;
out vec2 texCoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform int colorCycle = 2;
uniform ivec3 dim;

layout(r32f, binding = 0) uniform image3D voxels_temp_in;

// Function to calculate the 3D coordinates from a 1D index
ivec3 getCoordinates(uint index) {
	uint vz = index / (dim.x * dim.y);
	index -= (vz * dim.x * dim.y);
	uint vy = index / dim.x;
	uint vx = index % dim.x;
    return ivec3(vx, vy, vz);
}

float getTemp(uint i){
    return imageLoad(voxels_temp_in, getCoordinates(i)).r;
}

void main() {
	int i = gl_InstanceID;
	if (i >= numVoxels) return;
	if(ssbo_position[i].w != 1) return;

	vec3 offset = xi;
	position = vec3(model * vec4(_position + offset, 1.0f));
	normal = _normal;

	color = vec4(1);
	if(colorCycle == 1){
		color.rgb = (normalize(offset)+vec3(1)) / 2.0;
	}else if(colorCycle == 2){
		color  = colorMap(float((getTemp(i) - 298.15)/298.15), inferno);
	}

	gl_Position = projection * view * vec4(position, 1.0f);
}