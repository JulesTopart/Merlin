#version 430
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
uniform uint particleTest = 50;
uniform int showBoundary;
uniform vec2 WindowSize;
uniform float zoomLevel = 20;

void main() {
	vec2 offset = particles[gl_InstanceID].position;
	position = model * (vec4(_position + vec3(offset,0),1));

	uint testsortedID = sortedIndices[particleTest];
	
	bool binTest = true;
	bool nnTest = false;
	bool hTest = false;
	uint binindex = particles[gl_InstanceID].binIndex;//getBinIndex(particles[sortedID].position);

	bool test = false;//particles[sortedID].phase == UNUSED || (particles[sortedID].phase == BOUNDARY && showBoundary == 0);
	color = vec4(1);
	if(colorCycle == 0){ 
		color = vec4(vec3(0.8), 1.0);
	}else if(colorCycle == 1){ 
		color = vec4(randomColor(binindex), 1);
	}else if(colorCycle == 2){ 
		color = colorMap(map(particles[gl_InstanceID].density,0.9 * restDensity, 1.1 * restDensity), warmcool);
	}else if(colorCycle == 3){ 
		color = vec4(randomColor(binindex), 1);
	}else if(colorCycle == 4){ 
		color = colorMap(map(length(particles[gl_InstanceID].velocity),0,1000.0), parula);
	}else if(colorCycle == 5){ 
		color = vec4(randomColor(binindex), 1);
	}else{ //NNS Test
		if(gl_InstanceID == testsortedID){
			color = vec4(1,0,0, 1);
			binTest = true;
		}else{

			binTest = false;
			uvec2 binIndexVec2 = getBinCoord(particles[testsortedID].position);
			for (int y = int(binIndexVec2.y) - 1; y <= int(binIndexVec2.y) + 1; y++) {
				for (int x = int(binIndexVec2.x) - 1; x <= int(binIndexVec2.x) + 1; x++) {
					if (x < 0 || y < 0) continue;
					if (x >= binMax.x || y >= binMax.y) continue; 
					if (getBinIndexFromCoord(uvec2(x,y)) == binindex) binTest = true;
				}
			}
		

			uint i = testsortedID;
			OVERNNS
				if(gl_InstanceID == j){
					nnTest = true;
					if(length(particles[testsortedID].position - particles[j].position) <= smoothingRadius) hTest = true;
				}
			OVERNNS_END

			color = vec4(0,0,0, 1);
			if(nnTest) color = vec4(0,0,1, 1);
			if(hTest) color = vec4(0,1,0, 1);
		}
		
	}

	if( test || !binTest){
		screen_position = projection * view * vec4(0,0,0,1);
		gl_Position = screen_position;
		gl_PointSize = 0;
	}else{
		screen_position = projection * view * position;
		mv = projection * view;
		
		gl_Position = screen_position;
		gl_PointSize = particleRadius * WindowSize.y * 1.0 / zoomLevel;
		if(colorCycle == 6 && !hTest && !(gl_InstanceID == testsortedID)) gl_PointSize = particleRadius * WindowSize.y * 0.3 / zoomLevel;
		
	}
}