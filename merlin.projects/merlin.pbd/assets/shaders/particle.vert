#version 450
#include "common/uniforms.glsl"
#include "common/constants.glsl"
#include "common/buffers.glsl"

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;

out vec4 position;
out vec4 screen_position;
out vec4 normal;
out vec4 color;
out vec2 texCoord;
out mat4 mv;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform int colorCycle = 0;
uniform uint particleTest = 2000;
uniform int showBoundary;
uniform uint colorCount;
uniform vec2 WindowSize;

layout(std430, binding = 3) buffer ColorMapBuffer {
    vec4 colors[];
};

vec4 heatMap(const float value) {
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float v = (value - minValue)/(maxValue - minValue);
	vec4 color;
	color.a = 1.0f;
	if (colorCount == 0) return vec4(0,0,0,1);

	for (int i = 0; i < colorCount; i++) {
		vec4 currC = colors[i];
		if (v <= currC.w) {
			vec4 prevC = colors[max(0, i - 1)];
			float valueDiff = (prevC.w - currC.w);
			float fractBetween = (valueDiff == 0) ? 0 : (v - currC.w) / valueDiff;
			color.r = (prevC.r - currC.r) * fractBetween + currC.r;
			color.g = (prevC.g - currC.g) * fractBetween + currC.g;
			color.b = (prevC.b - currC.b) * fractBetween + currC.b;
			return color;
		}
	}
	color = colors[colorCount - 1];
	return color;
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


float rand(vec2 co){
    return 0.00001*abs(fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453));
}

vec3 randomColor(uint index){
	float v = float(index)/500.0;
	vec2 co = vec2(v, v*v);
	return vec3(0.2) + normalize(vec3(rand(co*0.8738), rand(co*0.321313), rand(0.12354*co)));
}



void main() {
	if (gl_InstanceID >= numParticles){
		gl_PointSize = 0;
		gl_Position =  projection * view * vec4(0,0,0,1);
		return;
	}

	vec3 offset = particles[gl_InstanceID].position.xyz;
	position = model * (vec4(_position + offset,1));


	
	bool binTest = true;
	bool nnTest = false;
	bool hTest = false;

	bool test = particles[gl_InstanceID].phase == UNUSED || (particles[gl_InstanceID].phase == BOUNDARY && showBoundary == 0);

	if(colorCycle == 0){ 
		color = vec4(vec3(0.8), 1.0);
	}else if(colorCycle == 1){ 
		color = vec4(randomColor(particles[gl_InstanceID].binIndex), 1);
	}/*else if(colorCycle == 2) {
		ColorScale rest;
		rest.minValue = int(0);
		rest.maxValue = int(2.0);
		//color = stableMap(map(int(particles[gl_InstanceID].density), colorScale[DENSITY_FIELD] ));
		//color = stableMap(map(int(particles[gl_InstanceID].density), rest ));
		//color = heatMap(map(int(particles[gl_InstanceID].density), rest ));
		color = heatMap(map(particles[gl_InstanceID].density, colorScale[DENSITY_FIELD]));
	}*/else if(colorCycle == 3) {
		ColorScale trest;
		trest.minValue = int(ambientTemperature*100.0);
		trest.maxValue = int(nozzleTemperature*100.0);

		//color = heatMap(map(particles[gl_InstanceID].temperature, trest));
		color = heatMap(map(int(particles[gl_InstanceID].temperature), colorScale[TEMPERATURE_FIELD]));
	}else if(colorCycle == 4) {
		color = heatMap(map(abs(particles[gl_InstanceID].lambda), colorScale[LAMBDA_FIELD]));
	}else if(colorCycle == 5) {
		color = heatMap(map(particles[gl_InstanceID].mass, colorScale[VELOCITY_FIELD]));
	}else{ //NNS Test
		
		binTest = true;
		uvec3 binIndexVec2 = getBinCoordFromIndex(particles[particleTest].binIndex);
		for (int z = int(binIndexVec2.z) - 1; z <= int(binIndexVec2.z) + 1; z++) {
			for (int y = int(binIndexVec2.y) - 1; y <= int(binIndexVec2.y) + 1; y++) {
				for (int x = int(binIndexVec2.x) - 1; x <= int(binIndexVec2.x) + 1; x++) {
					if (x < 0 || y < 0 || z < 0) continue;
					if (x >= binMax.x || y >= binMax.y || z >= binMax.z) continue; 

					if (getBinIndexFromCoord(uvec3(x,y,z)) == particles[gl_InstanceID].binIndex) binTest = true;
				}
			}
		}

		uint i = particleTest;
		OVERNNS
			if(gl_InstanceID == j){
				nnTest = true;
				if(length(particles[particleTest].position - particles[j].position) <= smoothingRadius) hTest = true;
			}
		OVERNNS_END

		color = vec4(0,0,0, 1);
		if(nnTest) color = vec4(0,0,1, 1);
		if(hTest) color = vec4(0,1,0, 1);
		if(gl_InstanceID == particleTest) color = vec4(1,0,0, 1);
	}

	if( test || !binTest){
		screen_position = projection * view * vec4(0,0,0,1);
		gl_Position = screen_position;
		gl_PointSize = 0;
	}
	else{
		screen_position = projection * view * position;
		mv = projection * view;
		
		gl_Position = screen_position;
		gl_PointSize = particleRadius * WindowSize.y * 1.8 / (gl_Position.w);
		if(colorCycle == 6 && !hTest && !(gl_InstanceID == particleTest)) gl_PointSize = 100.0/(gl_Position.w);
		
	}
}