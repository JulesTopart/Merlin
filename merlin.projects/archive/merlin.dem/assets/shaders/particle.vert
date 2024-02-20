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
	}else if(colorCycle == 2) {
		//color = stableMap(map(int(particles[gl_InstanceID].density), colorScale[DENSITY_FIELD] ));
		//color = stableMap(map(int(particles[gl_InstanceID].density), rest ));
		//color = heatMap(map(int(particles[gl_InstanceID].density), rest ));
		color = colorMap(map(particles[gl_InstanceID].density, 0.0 ,REST_DENSITY), jet);
	}else if(colorCycle == 3) {

		float minValue = ambientTemperature;
		float maxValue = nozzleTemperature*3.0;

		//color = heatMap(map(particles[gl_InstanceID].temperature, trest));
		color = colorMap(map(particles[gl_InstanceID].temperature, minValue, maxValue), jet);
		
	}else if(colorCycle == 4) {
		float minValue = 0.0;
		float maxValue = 20;
		color = colorMap(map(particles[gl_InstanceID].lambda, minValue, maxValue), jet);

	}else if(colorCycle == 5) {
		float minValue = 0.0;
		float maxValue = 5;
		color = colorMap(map(particles[gl_InstanceID].mass, minValue, maxValue), jet);
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