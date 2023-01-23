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
uniform double dt;
uniform vec3 hpos;

#define M_PI 3.14159265358979323846
#define PI_FAC 0.454728408833987

// Smoothing kernel function (Quntic kernel)
// Smoothing kernel function (Quntic kernel)
float lapl_pse(vec3 pi, vec3 pj, float hi) {
	float r = length(pj - pi);

	float h2 = hi*hi;
	float h4 = h2*h2;
	float w2_pse  = +4./(h4*M_PI)*exp(-r*r/(h2));

	return w2_pse;
}

float getTemp(ivec2 p, ivec2 offset){
	p += offset;
	uint ni = p.y * sqNodeCount + p.x;
	return nodes[ni].T;
}

void main() {
	uint index = gl_GlobalInvocationID.x;
	ivec3 p = ivec3(index / (sqNodeCount * sqNodeCount), (index % (sqNodeCount * sqNodeCount)) / sqNodeCount, index % sqNodeCount);

	Node n = nodes[index];

	if(length(hpos - n.U) < (0.05 * 2.5)/sqNodeCount){
		n.enable = 1;
		n.T = 200.0f / ((length(hpos - n.U)) / ((0.05 * 4)/sqNodeCount));
	}

	if(n.enable == 0) return;
	
	float mass = 2700.0f * 6.25f * pow(10, -6);
	float Ti = n.T;
	float T_ti = 0.;
	vec3 pi = n.U;
	for (int j = 0; j < nodeCount; j++) {
		vec3 pj   = nodes[j].U;
		float Tj = nodes[j].T;
		float rhoj = nodes[j].d;
    
		float w2_pse = lapl_pse(pi, pj, n.h);
		T_ti += (Tj-Ti)*w2_pse*(mass/rhoj);
	}
	n.T_t = T_ti;
	n.T += float(double((8.97*pow(10,-5)) * T_ti) * dt);

	if(p.z == 0){
		n.T = 60.0;
	}

	nodes[index] = n;
}