#version 450

#include "constants.glsl"
#include "common.glsl"

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;

out vec4 position;
out vec4 normal;
out vec4 color;
out vec2 texCoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;


uniform uint colorCount;

layout(std430) buffer ColorMapBuffer {
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


float rand(vec2 co){
    return 0.00001*abs(fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453));
}

vec3 randomColor(uint index){
	float v = float(index)/5000.0;
	vec2 co = vec2(v, v*v);
	return vec3(0.2) + normalize(vec3(rand(co*0.8738), rand(co*0.321313), rand(0.12354*co)));
}

uniform int colorCycle;
uniform int showBoundary;
uniform uint numParticles;

void main() {
	if (gl_InstanceID >= numParticles){
		gl_PointSize = 0;
		gl_Position =  projection * view * vec4(0,0,0,1);
		return;
	}

	vec3 offset = particles[gl_InstanceID].position.xyz*scale;
	position = model * (vec4(_position + offset,1));


	
	if( particles[gl_InstanceID].phase == BOUNDARY && showBoundary == 1 ) color = vec4(0,0,0,1);
	else {
		if(colorCycle == 0){ 
			color = vec4(randomColor(particles[gl_InstanceID].binIndex), 1);
		}else if(colorCycle == 1) {
			color = heatMap(particles[gl_InstanceID].density / REST_DENSITY);
			//else color = heatMap(particles[gl_InstanceID].temperature/nozzleTemperature);

			//color = vec4(randomColor(gl_InstanceID), 1);
			//color = vec3(length(particles[gl_InstanceID].velocity)/100.0);
			
		}else if(colorCycle == 2) {
			color = heatMap(particles[gl_InstanceID].temperature / REST_DENSITY);
			//else color = heatMap(particles[gl_InstanceID].temperature/nozzleTemperature);

			//color = vec4(randomColor(gl_InstanceID), 1);
			//color = vec3(length(particles[gl_InstanceID].velocity)/100.0);
			
		}else{ //NNS Test
			color = vec4(0,0,0, 1);
			if(particles[gl_InstanceID].binIndex == particles[2000].binIndex) color = vec4(1,0,0, 1);
			
			bool test = false;
			uint gid = 2000;
			OVERNNS
				if(gl_InstanceID == j) test = true;
			OVERNNS_END

			if(test) color = vec4(0,1,0, 1);
			else color = vec4(0,0,0, 1);
			
			if(gl_InstanceID == 2000) color = vec4(0,0,1, 1);
		}
	}

	if(particles[gl_InstanceID].phase == UNUSED || (particles[gl_InstanceID].phase == BOUNDARY && showBoundary == 0) ){
		gl_Position =  projection * view * vec4(0,0,0,1);
		gl_PointSize = 0;
	}
	else{
		gl_Position = projection * view * position;
		gl_PointSize = 200.0/(log(gl_Position.w)*10);
	}
}