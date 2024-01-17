#include "common.glsl"
// --- Kernels ---
// Kernel Functions Precomputed Constants
#define H smoothingRadius // Kernel radius
#define H2 pow(H,2)
#define H3 pow(H,3)
#define H5 pow(H,5)
#define H6 pow(H,6)
#define H9 pow(H,9)

#define POLY6_COEFFICIENT		(1.566681471061f/H9)		//(315 / (64 * PI * h^9)) * (h^2 - |r|^2)^3
#define POLY6_GRAD_COEFFICIENT  (9.400088826365069f/H9)		// (945 / (32 * PI * h^9)) * (h^2 - |r|^2)^3
#define POLY6_LAPL_COEFFICIENT  (37.600355305460276f/H9)	// (945 / (32 * PI * h^9)) * (h^2 - |r|^2)^3

#define SPIKY_COEFFICIENT		(4.7746482927568605f/H6)
#define SPIKY_GRAD_COEFFICIENT (14.32394487827058f/H6)

#define WVISC_COEFFICIENT		(2.3873241463784303f/H3)		//(315 / (64 * PI * h^9)) * (h^2 - |r|^2)^3
#define WVISC_GRAD_COEFFICIENT  WVISC_COEFFICIENT		// (945 / (32 * PI * h^9)) * (h^2 - |r|^2)^3
#define WVISC_LAPL_COEFFICIENT  (14.32394487827058f/H5)	// (945 / (32 * PI * h^9)) * (h^2 - |r|^2)^3

#define M_PI 3.14159265358979323846
#define PI_FAC 0.454728408833987




// --- POLY6 KERNEL ---
//Computed the poly6 scalar smoothing kernel
float poly6(vec4 r){
	float r2 = length(r.xyz) * length(r.xyz);
	if (r2 < EPSILON || r2 > H2 || H9 < EPSILON) return 0.0;
	float B = H2 - r2;
	return POLY6_COEFFICIENT * (B * B * B);
}

//Computed the poly6 gradient smoothing kernel
vec4 poly6_grad(vec4 r){
	float r2 = length(r.xyz) * length(r.xyz);
	if (r2 < EPSILON || r2 > H2 || H9 < EPSILON) return vec4(0.0);
	float B = H2-r2;
	return -r *POLY6_GRAD_COEFFICIENT * (B * B);
}

//Computed the poly6 laplacian smoothing kernel
float poly6_lapl(vec4 r) {
	float r2 = length(r.xyz) * length(r.xyz);
	if (r2 < EPSILON || r2 > H2 || H9 < EPSILON) return 0.0;
	float B = H2 - r2;
	return POLY6_LAPL_COEFFICIENT * B * (r2 - (3.0f*B/4.0f));
}




// --- SPIKY KERNEL ---
//Computed the poly6 scalar smoothing kernel
float spiky(vec4 r) {
	float r2 = length(r.xyz) * length(r.xyz);
	if (r2 < EPSILON || r2 > H2 || H9 < EPSILON) return 0.0;
	float B = H2 - r2;
	return SPIKY_COEFFICIENT * (B * B * B);
}

//Computed the poly6 gradient smoothing kernel
vec4 spiky_grad(vec4 r) {
	float r2 = length(r.xyz) * length(r.xyz);
	if (r2 < EPSILON || r2 > H2 || H9 < EPSILON) return vec4(0.0);
	float B = H2 - r2;
	return -normalize(r) * SPIKY_GRAD_COEFFICIENT * (B * B);
}




// --- VISCOSITY KERNEL ---
//Computed the poly6 scalar smoothing kernel
float Wvisc(vec4 r_v) {
	float r = length(r_v.xyz);
	float r2 = r * r;
	float r3 = r2 * r;
	if (r2 < EPSILON || r2 > H2 || H9 < EPSILON) return 0.0;
	float A = -r3 / (2 * H3);
	float B =  r2 / H2;
	float C =  H / (2 * r);
	return WVISC_COEFFICIENT * (A + B + C - 1);
}

//Computed the Wvisc gradient smoothing kernel
vec4 Wvisc_grad(vec4 r_v) {
	float r = length(r_v.xyz);
	float r2 = r * r;
	float r3 = r2 * r;
	if (r2 < EPSILON || r2 > H2 || H9 < EPSILON) return vec4(0.0);
	float A = (-3 * r) / (2 * H3);
	float B = 2 / H2;
	float C = -H / (2 * r3);
	return -r_v * WVISC_GRAD_COEFFICIENT * (A + B + C);
}

//Computed the Wvisc laplacian smoothing kernel
float Wvisc_lapl(vec4 r_v) {
	float r = length(r_v.xyz);
	if (r < EPSILON || r > H || H9 < EPSILON) return 0.0;
	
	return WVISC_LAPL_COEFFICIENT * (1 - (r/H));
}