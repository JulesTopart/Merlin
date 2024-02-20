#version 450
layout (local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

struct Node {
    vec3 U;		//Position
	vec3 V;		//Velocity

	float h;	//Smoothing radius
	float d;	//Density
	float p;	//Pressure
	float T;	//Temperature

	vec4 S;		//stress [Sxx, Sxy, Syy, Szz]
	vec4 R;		//artificial Stress [Rxx, Rxy, Ryy, -]
	
	vec4 S_d;	//dS(u)/du Stress spatial derivative
	vec4 V_d;	//dV(u)/du ? spatial derivative

	//plastic strain and strain rate
	float eps_pl;
	float eps_pl_dot;

	//Time derivatives
	vec3 U_t;	//dU(t)/dt
	vec3 V_t;	//dV(t)/dt

	float d_t;	//dd(t)/dt
	vec4  S_t;	//dS(t)/dt
	float T_t;	//dT(t)/dt

	//Boundary condition
	float fix;				//particle is fixed in space
	float enable;			//particle is deactivated
};

layout (std430, binding = 1) buffer NodeBuffer {
  Node nodes[];
};

uniform uint nodeCount;
uniform uint sqNodeCount;

void main() {
  uint index = gl_GlobalInvocationID.x;
  // Calculate the indices for each dimension using div and mod
  
  Node n = nodes[index];

  n.h = 0.05/30.0f;	//Smoothing radius
  n.d = 2700.0;	//Density
  n.T = 20.0;	//Temperature

  ivec2 c = ivec2( index % sqNodeCount, index / sqNodeCount);

  if(c.y == 0){
	n.T = 20.0;
  }
  if(c.y == sqNodeCount-1){
	n.T = 60.0;
  }

  nodes[index] = n;
}