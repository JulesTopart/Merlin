#version 450
#include "common/uniforms.comp"
#include "common/constants.comp"
#include "common/buffers.comp"
#include "common/nns.comp"
#include "common/colors.comp"

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
uniform vec2 WindowSize;


void main() {
	if (gl_InstanceID >= numParticles){
		gl_PointSize = 0;
		gl_Position =  projection * view * vec4(0,0,0,1);
		return;
	}

	vec2 offset = particles[gl_InstanceID].position;
	position = model * (vec4(_position + vec3(offset,0),1));

	bool binTest = true;
	bool nnTest = false;
	bool hTest = false;
	uint binindex = getBinIndex(particles[gl_InstanceID].position);

	bool test = particles[gl_InstanceID].phase == UNUSED || (particles[gl_InstanceID].phase == BOUNDARY && showBoundary == 0);

	if(colorCycle == 0){ 
		color = vec4(vec3(0.8), 1.0);
	}else if(colorCycle == 1){ 
		
		color = vec4(randomColor(binindex), 1);
	}else{ //NNS Test
		
		binTest = true;
		uvec2 binIndexVec2 = getBinCoord(particles[gl_InstanceID].position);
		for (int y = int(binIndexVec2.y) - 1; y <= int(binIndexVec2.y) + 1; y++) {
			for (int x = int(binIndexVec2.x) - 1; x <= int(binIndexVec2.x) + 1; x++) {
				if (x < 0 || y < 0) continue;
				if (x >= binMax.x || y >= binMax.y) continue; 

				if (getBinIndexFromCoord(uvec2(x,y)) == binindex) binTest = true;
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
		gl_PointSize = particleRadius * WindowSize.y * 0.09;
		if(colorCycle == 6 && !hTest && !(gl_InstanceID == particleTest)) gl_PointSize = particleRadius * WindowSize.y * 0.25*1e-3;
		
	}
}