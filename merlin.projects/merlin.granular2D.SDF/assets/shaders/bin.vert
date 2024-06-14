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
	vec2 offset = (vec2(getBinCoordFromIndex(gl_InstanceID) + vec2(0.5)) * binSize) + boundaryMin;
	position = vec3(model * vec4(_position*0.98 + vec3(offset,0) , 1.0f));
	normal = _normal;

	bool test = true || bins[gl_InstanceID].count > 0;

	color = vec4(1);
	if(colorCycle == 1){
		color = vec4(randomColor(gl_InstanceID), 1);
	}else if(colorCycle != 6){
		color = colorMap(float(bins[gl_InstanceID].count)/(MAXNN), jet);
	}else {
		
		if(gl_InstanceID == binTest) color = vec4(1,0,0,1);
		else{
			test = false;
			uint testsortedID = B_SortedID(particleTest);

			uvec2 binIndexVec = getBinCoord(B_X(testsortedID));
			ivec2 minBound = max(ivec2(binIndexVec) - 1, ivec2(0));
			ivec2 maxBound = min(ivec2(binIndexVec) + 1, ivec2(binMax) - 1);
			for (int y = minBound.y; y <= maxBound.y; y++) {
				for (int x = minBound.x; x <= maxBound.x; x++) {
					uint cindex = getBinIndexFromCoord(uvec2(x, y));
						if (cindex == gl_InstanceID) test = true;
					}
				}
			if(test)color = vec4(0.2,1,0.2,1);
		}
	}

	if(!test)gl_Position = vec4(0);
	else gl_Position = projection * view * vec4(position, 1.0f);
}