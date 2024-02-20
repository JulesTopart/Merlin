#version 450

#include "common/uniforms.glsl"
#include "common/constants.glsl"
#include "common/buffers.glsl"

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;

out vec3 position;
out vec3 normal;
out vec4 color;
out vec2 texCoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform uint particleTest = 2000;
uniform uint binTest = 1459;

float rand(vec2 co){
    return 0.00001*abs(fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453));
}

vec3 randomColor(uint index){
	float v = float(index)/500.0;
	vec2 co = vec2(v, v*v);
	return vec3(0.2) + normalize(vec3(rand(co*0.8738), rand(co*0.321313), rand(0.12354*co)));
}

struct Color {
    vec3 color;
    float value;
};

layout(std430, binding = 0) buffer ColorMapBuffer {
    Color colors[];
};

uniform uint colorCount;

vec4 heatMap(const float value) {
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float v = (value - minValue) / (maxValue - minValue);
	vec4 color;
	color.a = 1.0f;
	if (colorCount == 0) return vec4(0,0,0,1);

	for (int i = 0; i < colorCount; i++) {
		Color currC = colors[i];
		if (v <= currC.value) {
			Color prevC = colors[max(0, i - 1)];
			float valueDiff = (prevC.value - currC.value);
			float fractBetween = (valueDiff == 0) ? 0 : (v - currC.value) / valueDiff;
			color.r = (prevC.color.r - currC.color.r) * fractBetween + currC.color.r;
			color.g = (prevC.color.g - currC.color.g) * fractBetween + currC.color.g;
			color.b = (prevC.color.b - currC.color.b) * fractBetween + currC.color.b;
			return color;
		}
	}
	color = vec4(colors[colorCount - 1].color, 1.0);
	return color;
}

float near = 0.1; 
float far  = 90.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


uniform int colorCycle;

void main() {
	vec3 offset = (vec3(getBinCoordFromIndex(gl_InstanceID)) * binSize);
	position = vec3(model * vec4(_position*0.95 + offset + boundaryMin, 1.0f));
	normal = _normal;

	bool test = bins[gl_InstanceID].count > 0;

	color = vec4(1);
	if(colorCycle == 0){
		color = vec4(randomColor(gl_InstanceID), 1);
	}else if(colorCycle != 5){
		color = heatMap(float(bins[gl_InstanceID].count)/(MAXNN));
	}else {
		
		if(gl_InstanceID == binTest) color = vec4(1,0,0,1);
		else{
			test = false;
			uvec3 binIndexVec = getBinCoordFromIndex(particles[particleTest].binIndex);
			for (int z = int(binIndexVec.z) - 1; z <= int(binIndexVec.z) + 1; z++) {
				for (int y = int(binIndexVec.y) - 1; y <= int(binIndexVec.y) + 1; y++) {
					for (int x = int(binIndexVec.x) - 1; x <= int(binIndexVec.x) + 1; x++) {
						if (x < 0 || y < 0 || z < 0) continue;
						if (x >= binMax.x || y >= binMax.y || z >= binMax.z) continue; 

						if (getBinIndexFromCoord(uvec3(x,y,z)) == gl_InstanceID) test = true;
					}
				}
			}
			if(test)color = vec4(0.2,1,0.2,1);
		}
	}

	if(!test)gl_Position = vec4(0);
	else gl_Position = projection * view * vec4(position, 1.0f);
}