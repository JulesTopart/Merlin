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

layout(std430, binding = 4) buffer ColorMapBuffer {
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
	vec3 offset = vec3(0);
	
	if(length(_position) == 0){
		offset = (vec3(particles[0].position));
	}else{
		offset = vec3(particles[1].position);
	}
	
	position = vec3(model * vec4(_position + offset + boundaryMin, 1.0f));
	normal = _normal;

	color = vec4(1);

	gl_Position = projection * view * vec4(position, 1.0f);
}