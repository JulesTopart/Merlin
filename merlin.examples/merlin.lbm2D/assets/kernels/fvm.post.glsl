#version 460

layout(local_size_x = 1, local_size_y = 1) in;

layout(rg32f, binding = 0) uniform image2D velocity;	//Velocity field u velocity, v velocity
layout(r32f, binding = 1) uniform image2D phase;		//Phase field
layout(r32f, binding = 2) uniform image2D pressure;		//Pressure field


void main() {
	ivec2 iPos = ivec2(gl_GlobalInvocationID.xy);

	vec4 U = imageLoad(velocity, iPos);
	U.w = 0.0;

	imageStore(velocity, iPos, U);
} 