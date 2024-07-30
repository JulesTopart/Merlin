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

double computeVonMisesStress(sym_tensor3x3 sigma) {

    double vonMises = sqrt(
        0.5 * (
            (sigma.xx - sigma.yy, 2.0)*(sigma.xx - sigma.yy, 2.0) + 
            (sigma.yy - sigma.zz, 2.0)*(sigma.yy - sigma.zz, 2.0) + 
            (sigma.zz - sigma.xx, 2.0)*(sigma.zz - sigma.xx, 2.0) + 
            6.0 * (sigma.xy*sigma.xy + sigma.yz*sigma.yz + sigma.xz*sigma.xz)
        )
    );

    return vonMises;
}

void main() {
	uint i = gl_InstanceID;
	vec3 offset = vec3(b_xi.xyz);
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
		color = colorMap(map(float(b_rho_i), 0.99 * float(u_rho0), 1.01 * float(u_rho0)), warmcool);
	}else if(colorCycle == 3){ 
		color = colorMap(map(float(computeVonMisesStress(ssbo_sigma[i])),0, 2e6), jet);//stress
		//color = vec4(ssbo_sigma[i].xx, ssbo_sigma[i].yy, ssbo_sigma[i].zz, 1);///stress
	}else if(colorCycle == 4){ 
		color = colorMap(map(float(length(b_vi)),0,1000.0), parula);
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

			dvec3 position = ssbo_last_position[testsortedID].xyz;
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
		gl_PointSize = 4.0*float(cst_particleRadius)*400.0/(gl_Position.w);
		if(colorCycle == 6 && !hTest && !(gl_InstanceID == particleTest)) gl_PointSize = 400.0/(gl_Position.w);
		
	}
}