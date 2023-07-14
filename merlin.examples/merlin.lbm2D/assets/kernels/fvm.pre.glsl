#version 460

layout(local_size_x = 1, local_size_y = 1) in;

layout(rg32f, binding = 0) uniform image2D velocity;	//Velocity field u velocity, v velocity
layout(r32f, binding = 1) uniform image2D phase;		//Phase field
layout(r32f, binding = 2) uniform image2D pressure;		//Pressure field

bool isSolid(ivec2 pos){
	vec4 P = imageLoad(phase, pos);
	return P.x == 0.0;
}

void main() {
	ivec2 iPos = ivec2(gl_GlobalInvocationID.xy);
	if(isSolid(iPos)) return;
	imageStore(pressure, iPos, vec4(0.0));
} 