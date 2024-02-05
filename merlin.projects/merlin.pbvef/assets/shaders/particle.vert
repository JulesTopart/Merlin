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

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform int colorCycle;
uniform uint particleTest = 2000;
uniform int showBoundary;


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
	uint binindex = particles[gl_InstanceID].binIndex;//getBinIndex(particles[sortedID].position);
	uint testsortedID = particleTest;
	bool test = particles[gl_InstanceID].phase == UNUSED || (particles[gl_InstanceID].phase == BOUNDARY && showBoundary == 0);

	if(colorCycle == 0){ 
		color = vec4(vec3(0.8), 1.0);
	}else if(colorCycle == 1){ 
		color = vec4(randomColor(binindex), 1);
	}else if(colorCycle == 2){ 
		color = colorMap(map(particles[gl_InstanceID].density,0.8 * REST_DENSITY, 1.2 * REST_DENSITY), warmcool);
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
			uvec3 binIndexVec3 = getBinCoord(particles[testsortedID].position);
			for (int z = int(binIndexVec3.y) - 1; z <= int(binIndexVec3.z) + 1; z++) {
			for (int y = int(binIndexVec3.y) - 1; y <= int(binIndexVec3.y) + 1; y++) {
			for (int x = int(binIndexVec3.x) - 1; x <= int(binIndexVec3.x) + 1; x++) {
				if (x < 0 || y < 0 || z < 0) continue;
				if (x >= binMax.x || y >= binMax.y || z >= binMax.z) continue; 
				if (getBinIndexFromCoord(uvec3(x,y,z)) == binindex) binTest = true;
			}}}
		

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
	}
	else{
		screen_position = projection * view * position;
		gl_Position = screen_position;
		gl_PointSize = 2.0*particleRadius*400.0/(gl_Position.w);
		if(colorCycle == 5 && !hTest && !(gl_InstanceID == particleTest)) gl_PointSize = 100.0/(gl_Position.w);
		
	}
}