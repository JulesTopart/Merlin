#version 450

#include "common/uniforms.comp"
#include "common/constants.comp"
#include "common/buffers.comp"
#include "common/nns.comp"
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

uniform uint particleTest = 2000;
uniform uint binTest = 1459;

uniform int colorCycle;

void main() {
	vec3 offset = (vec3(getBinCoordFromIndex(gl_InstanceID)) * binSize);
	position = vec3(model * vec4(_position*0.95 + offset + boundaryMin, 1.0f));
	normal = _normal;

	bool test = bins[gl_InstanceID].count > 0;

	color = vec4(1);
	if(colorCycle == 1){
		color = vec4(randomColor(gl_InstanceID), 1);
	}else if(colorCycle != 6){
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