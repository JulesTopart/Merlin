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

uniform int colorCycle = 4;
uniform uint particleTest = 50;
uniform int showBoundary = 0;
uniform vec2 WindowSize;
uniform float zoomLevel = 20;

float computeVonMisesStress(sym_tensor3x3 sigma) {

    float vonMises = sqrt(
        0.5 * (
            pow(sigma.xx - sigma.yy, 2.0) + 
            pow(sigma.yy - sigma.zz, 2.0) + 
            pow(sigma.zz - sigma.xx, 2.0) + 
            6.0 * (pow(sigma.xy, 2.0) + pow(sigma.yz, 2.0) + pow(sigma.xz, 2.0))
        )
    );

    return vonMises;
}

void main() {
	uint i = gl_InstanceID;
	vec3 offset = b_xi.xyz;
	position = model * (vec4(_position + vec3(offset),1));

	uint testID = id(particleTest);
	uint testsortedID = sorted_id(particleTest);
	
	bool binTest = true;
	bool nnTest = false;
	bool hTest = false;
	uint binindex = bin_index(gl_InstanceID);//getBinIndex(particles[sortedID].position);

	bool test = phase(gl_InstanceID) == UNUSED || (phase(gl_InstanceID) == BOUNDARY && showBoundary == 0);
	color = vec4(1);
	if(colorCycle == 0){ 
		color = vec4(vec3(0.8), 1.0);
	}else if(colorCycle == 1){ 
		color = vec4(randomColor(binindex), 1);
	}else if(colorCycle == 2){ 
		color = colorMap(map(b_rho_i*1000, 1000 * u_rho0, 1050 * u_rho0), warmcool);
	}else if(colorCycle == 3){ 
		
		float maxS = 500e6;

		float r = map(abs(ssbo_sigma[i].xx),-maxS, maxS);
		float g = map(abs(ssbo_sigma[i].yy),0, maxS);
		float b = map(abs(ssbo_sigma[i].zz),0, maxS);
		
		float r2 = map(abs(ssbo_sigma[i].xy),0, maxS);
		float g2 = map(abs(ssbo_sigma[i].yz),0, maxS);
		float b2 = map(abs(ssbo_sigma[i].xz),0, maxS);

		//color = vec4(r, g, b,  1.0);
		//color = vec4(r2, g2, b2,  1.0);

		//color = colorMap(r, warmcool);//stress
		color = colorMap(map(computeVonMisesStress(ssbo_sigma[i]),0, maxS), jet);//stress
	}else if(colorCycle == 4){ 
		color = colorMap(map(length(b_vi),0,1000.0), parula);
	}else if(colorCycle == 5){ 
		color = vec4(randomColor(binindex), 1);
	}else{ //NNS Test
		if(gl_InstanceID == testsortedID){
			color = vec4(1,0,0, 1);
			binTest = true;
		}else{

			binTest = false;
			uvec3 binIndexVec3 = getBinCoord(ssbo_last_position[testsortedID].xyz);
			for (int z = int(binIndexVec3.z) - 1; z <= int(binIndexVec3.z) + 1; z++) {
				for (int y = int(binIndexVec3.y) - 1; y <= int(binIndexVec3.y) + 1; y++) {
					for (int x = int(binIndexVec3.x) - 1; x <= int(binIndexVec3.x) + 1; x++) {
						if (x < 0 || y < 0) continue;
						if (x >= cst_binMax.x || y >= cst_binMax.y) continue; 
						if (getBinIndexFromCoord(uvec3(x,y,z)) == binindex) binTest = true;
					}
				}
			}

			vec3 position = ssbo_last_position[testsortedID].xyz;
			OVERNNS
				if(gl_InstanceID == j){
					nnTest = true;
					if(distance(ssbo_last_position[testsortedID].xyz,  ssbo_last_position[j].xyz) < cst_smoothingRadius) hTest = true;
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
		gl_PointSize = 4.0*cst_particleRadius*400.0/(gl_Position.w);
		if(colorCycle == 6 && !hTest && !(gl_InstanceID == particleTest)) gl_PointSize = 400.0/(gl_Position.w);
		
	}
}