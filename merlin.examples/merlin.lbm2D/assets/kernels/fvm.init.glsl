#version 460

layout(local_size_x = 1, local_size_y = 1) in;

layout(rg32f, binding = 0) uniform image2D velocity;	//Velocity field u velocity, v velocity
layout(r32f, binding = 1) uniform image2D phase;		//Phase field
layout(r32f, binding = 2) uniform image2D pressure;		//Pressure field


uniform float width;
uniform float height;

void main() {
	ivec2 iPos = ivec2(gl_GlobalInvocationID.xy);

	float u = 0.1;
	float v = 0.0;

	imageStore(velocity, iPos, vec4(u, v, 0, 0));
	


	float s = 1.0;
	if (iPos.x == 0 || iPos.y == 0 || iPos.x == width-1 || iPos.y == height-1) //borders
		s = 0.0;
	if (iPos.x > 100 && iPos.y >(height /2)-50 && iPos.x <200 && iPos.y < (height / 2) + 50) //Square
		s = 0.0;

	imageStore(pressure, iPos, vec4(s == 0.0 ? 1.0 : 0.0, 0, 0, 0));
	imageStore(phase, iPos, vec4(s,0,0,0));
	
}