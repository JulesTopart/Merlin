
// --- SPH_Kernels ---
//Computed the poly6 scalar smoothing kernel
float poly6(vec3 r)
{
	float rBar = length(r);

	if (rBar < EPSILON || rBar > H) {
		return EPSILON;
	}

	// (315 / (64 * PI * h^9)) * (h^2 - |r|^2)^3
	if (H9 < EPSILON) {
		return 0.0f;
	}
	float A = 1.566681471061f / H9;
	float B = (H2) - (rBar * rBar);

	return A * (B * B * B);
}

//Computes the spiky smoothing kernel gradient*/
vec3 spiky(vec3 r)
{
	float rBar = length(r);

	if (rBar < EPSILON || rBar > H) {
		return vec3(0.0f, 0.0f, 0.0f);
	}

	// (45 / (PI * h^6)) * (h - |r|)^2 * (r / |r|)
	if (H6 < EPSILON) {
		return vec3(0.0f, 0.0f, 0.0f);
	}
	float A = 14.323944878271f / H6;
	float B = (H - rBar);
	vec3 outGrad = A * (B * B) * (r / (rBar + EPSILON));
	return outGrad;
}

// Computes the viscosity Laplacian
float viscosityLaplacianKernel(float rLength)
{
	if (rLength <= H)
	{
		return VISC_LAPLACE_COEFFICIENT * (H - rLength);
	}
	return 0.0;
}

// Computes the Pressure Laplacian
float lapl_pse(vec3 pi, vec3 pj, float hi) {
	float r = length(pi - pj);

	float h2 = hi*hi;
	float h4 = h2*h2;
	float w2_pse  = +4./(h4*M_PI)*exp(-r*r/(h2));

	return w2_pse;
}
