#version 430

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

uniform uint particleTest = 50;
uniform uint binTest = 1459;

uniform int colorCycle;

void main() {
	vec3 offset = (vec3(getBinCoordFromIndex(gl_InstanceID)) * cst_binSize);
	position = vec3(model * vec4(_position*0.95 + offset + cst_boundaryMin, 1.0f));
	normal = _normal;

	bool test = ssbo_bins[gl_InstanceID].count > 0;

	color = vec4(1);
	if(colorCycle == 1){
		color = vec4(randomColor(gl_InstanceID), 1);
	}else if(colorCycle != 6){
		color = colorMap(float(ssbo_bins[gl_InstanceID].count)/(MAXNN), jet);
	}else {
		
		if(gl_InstanceID == binTest) color = vec4(1,0,0,1);
		else{
			test = false;
			uint testsortedID = sorted_id(particleTest);

			uvec3 binIndexVec = getBinCoord(ssbo_predicted_position[testsortedID].xyz);
			ivec3 minBound = max(ivec3(binIndexVec) - 1, ivec3(0));
			ivec3 maxBound = min(ivec3(binIndexVec) + 1, ivec3(cst_binMax) - 1);
			for (int z = minBound.z; z <= maxBound.z; z++) {
				for (int y = minBound.y; y <= maxBound.y; y++) {
					for (int x = minBound.x; x <= maxBound.x; x++) {
						uint cindex = getBinIndexFromCoord(uvec3(x, y, z));
						if (cindex == gl_InstanceID) test = true;
					}
				}
			}
			if(test)color = vec4(0.2,1,0.2,1);
		}
	}

	if(!test)gl_Position = vec4(0);
	else gl_Position = projection * view * vec4(position, 1.0f);
}