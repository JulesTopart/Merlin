#include "common.glsl"
// --- Kernels ---
// Kernel Functions Precomputed Constants
#define H smoothingRadius // Kernel radius
#define H2 H * H
#define H6 H * H * H * H * H * H
#define H9 H * H * H * H * H * H * H * H * H
#define POLY6_COEFFICIENT 315.0 / (64.0 * 3.14159265359 * pow(H, 9))
#define SPIKY_GRAD_COEFFICIENT -45.0 / (3.14159265359 * pow(H, 6))
#define VISC_LAPLACE_COEFFICIENT 45.0 / (3.14159265359 * pow(H, 6))
#define M_PI 3.14159265358979323846
#define PI_FAC 0.454728408833987

// --- SPH_Kernels ---
//Computed the poly6 scalar smoothing kernel
float poly6(vec4 r)
{
	float rBar = length(r.xyz);

	if (rBar < EPSILON || rBar > H) {
		return 0.0;
	}

	// (315 / (64 * PI * h^9)) * (h^2 - |r|^2)^3
	if (H9 < EPSILON) {
		return 0.0f;
	}
	float A = 1.566681471061f / H9;
	float B = (H2) - (rBar * rBar);

	return A * (B * B * B);
}
#define poly6_0 poly6(vec4(0));

//Computes the spiky smoothing kernel gradient*/
vec4 spiky(vec4 r)
{
	float rBar = length(r.xyz);

	if (rBar < EPSILON || rBar > H) {
		return vec4(0.0f);
	}

	// (45 / (PI * h^6)) * (h - |r|)^2 * (r / |r|)
	if (H6 < EPSILON) {
		return vec4(0.0f);
	}
	float A = 14.323944878271f / H6;
	float B = (H - rBar);
	vec4 outGrad = A * (B * B) * (r / (rBar + EPSILON));
	return outGrad;
}
#define spiky_0 spiky(vec4(0));

