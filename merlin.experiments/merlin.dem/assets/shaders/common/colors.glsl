//? #version 430
#ifndef INCLUDE_COLORS_GLSL
#define INCLUDE_COLORS_GLSL

//?#include "../common/buffers.glsl"

#define LAMBDA_FIELD 0
#define DENSITY_FIELD 1
#define TEMPERATURE_FIELD 2
#define VELOCITY_FIELD 3
#define MAX_FIELD 3

#define NEAR 0.1
#define FAR  90.0

struct Color {
    vec3 color;
    float value;
};

layout(std430) buffer ColorMapBuffer {
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

float rand(vec2 co){
    return 0.00001*abs(fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453));
}

float map(float value, float minValue, float maxValue){
	return (value - minValue)/(maxValue - minValue);
}

vec3 randomColor(uint index){
	float v = float(index)/500.0;
	vec2 co = vec2(v, v*v);
	return vec3(0.2) + normalize(vec3(rand(co*0.8738), rand(co*0.321313), rand(0.12354*co)));
}

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));	
}


vec4 stableMap(const float value) {
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float v = (value - minValue)/(maxValue - minValue);
	vec4 color;
	color.a = 1.0f;
	if (colorCount == 0) return vec4(0,0,0,1);
		
		vec4 prevC = vec4(0,0,1,0.0);
		vec4 currC = vec4(1,1,1,0.5);
		
		if (v <= currC.w) {
			float valueDiff = (prevC.w - currC.w);
			float fractBetween = (valueDiff == 0) ? 0 : (v - currC.w) / valueDiff;
			color.r = (prevC.r - currC.r) * fractBetween + currC.r;
			color.g = (prevC.g - currC.g) * fractBetween + currC.g;
			color.b = (prevC.b - currC.b) * fractBetween + currC.b;
			return color;
		}

		prevC = currC;
		currC = vec4(1,0,0,1.0);
		
		if (v <= currC.w) {
			float valueDiff = (prevC.w - currC.w);
			float fractBetween = (valueDiff == 0) ? 0 : (v - currC.w) / valueDiff;
			color.r = (prevC.r - currC.r) * fractBetween + currC.r;
			color.g = (prevC.g - currC.g) * fractBetween + currC.g;
			color.b = (prevC.b - currC.b) * fractBetween + currC.b;
			return color;
		}
	
	color = vec4(0.0);
	return color;
}


#endif