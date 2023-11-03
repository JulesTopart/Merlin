#version 460
#pragma shader_stage(compute)

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct Particle {
	vec3 position;      // position (x, y, z)
	float mass;			// mass(w)

	vec3 velocity;      // velocity (x, y, z) and density (w)
	float density;		// density g/m3

	vec3 stressTensorX; // stress tensor (xx, xy, xz)
	float temperature;	// temperature (K)

	vec3 stressTensorY; // stress tensor (yx, yy, yz, 0)
	float pressure;		// pressure (Pa)

	vec3 stressTensorZ; // stress tensor (zx, zy, zz, 0)
	float strain;		// strain (Pa)
};

layout(std430, binding = 1) buffer ParticleBuffer {
	Particle particles[];
};


// SPH Parameters
//const float H = 0.045; // Kernel radius
const float H = 2.6 * 0.03125; // Kernel radius // 2.4*0.375
const float H2 = H * H;

const float REST_DENSITY = 1.0; //1000.0
const float STIFFNESS = 10.0; //70000.0
const float VISCOSITY = 0.0; //1000.0

// Kernel Functions Precomputed Constants
const float POLY6_COEFFICIENT = 315.0 / (64.0 * 3.14159265359 * pow(H, 9));
const float SPIKY_GRAD_COEFFICIENT = -45.0 / (3.14159265359 * pow(H, 6));
const float VISC_LAPLACE_COEFFICIENT = 45.0 / (3.14159265359 * pow(H, 6));

// Define boundary
const vec3 boundaryMin = vec3(0.0, 0.0, 0);
const vec3 boundaryMax = vec3(2, 1.5, 1.5);
const float boundaryRestitution = 0.05; // Bounce factor
const float boundaryRepulsionDistance = 0.03125;
const float boundaryRepulsionForce = 0.0;

uniform float dt;
uniform float speed;
uniform int numParticles;

// SPH_Kernels
float poly6Kernel(float rSquared)
{
	if (rSquared <= H2)
	{
		return POLY6_COEFFICIENT * pow(H2 - rSquared, 3);
	}
	return 0.0;
}

vec3 spikyGradientKernel(vec3 r){
	float rLength = length(r);
	if (rLength <= H && rLength > 0.0)
	{
		float spikyGrad = SPIKY_GRAD_COEFFICIENT * pow(H - rLength, 2);
		return (spikyGrad / rLength) * r;
	}
	return vec3(0.1);
}

float viscosityLaplacianKernel(float rLength)
{
	if (rLength <= H)
	{
		return VISC_LAPLACE_COEFFICIENT * (H - rLength);
	}
	return 0.0;
}


// SPH_Update
float calculateDensity(uint gid, vec3 position){
	float density = 0.0;

	for (uint j = 0; j < numParticles; j++) {
		if (j == gid) continue;

		Particle pj = particles[j];
		vec3 diff = position - pj.position;
		float rSquared = dot(diff, diff);

		density += pj.mass * poly6Kernel(rSquared);
	}

	return max(density, 0.1); //Update density
}


// SPH_BoundaryForce
vec3 calculateBoundaryForce(uint gid) {
	vec3 force = vec3(0);
	float distanceFromWall = boundaryRepulsionDistance * 2;

	for (int i = 0; i < 3; i++) {
		//X+
		float distanceFromWall = abs(particles[gid].position[i] - boundaryMax[i]);
		if (distanceFromWall < boundaryRepulsionDistance) {
			float distanceFromWall = particles[gid].position[i] - boundaryMax[i];
			distanceFromWall = abs(distanceFromWall);
			force[i] -= boundaryRepulsionForce * exp(((-1.0) * distanceFromWall) / boundaryRepulsionDistance);
		}
		//X-
		distanceFromWall = abs(particles[gid].position[i] - boundaryMin[i]);
		if (distanceFromWall < boundaryRepulsionDistance) {
			float distanceFromWall = particles[gid].position[i] - boundaryMin[i];
			distanceFromWall = abs(distanceFromWall);
			force[i] += boundaryRepulsionForce * exp(((-1.0) * distanceFromWall) / boundaryRepulsionDistance);
		}
	}
	return force;
}

float distanceConstraint(uint gid, uint jid, vec3 deltaPosition) {
	if (gid == jid) return 0.0;

    Particle p = particles[gid];
    Particle neighbor = particles[jid];

	float d = length(p.position - neighbor.position);
	if (d < 1e-12 || d >= H2) return 0;

    vec3 r = p.position + deltaPosition - neighbor.position;
    float dist = length(r);
    float restDistance = 0.03125; // Rest distance between particles
    // Calculate the distance constraint
    float C = (dist - restDistance) / dist;
    return C;
}

vec3 distanceConstraintGradient(uint gid, uint jid, vec3 deltaPosition) {
	if (gid == jid) return vec3(0.0);

    Particle p = particles[gid];
    Particle neighbor = particles[jid];

	float d = length(p.position - neighbor.position);
	if (gid == jid || d < 1e-12 || d >= H2) return vec3(0);

    vec3 r = p.position + deltaPosition - neighbor.position;

    vec3 gradient = -normalize(r);

    return gradient;
}


// SPH Gradient Function
vec3 applyDensityConstraints(uint gid, vec3 pposition, float density) {

	float omega = 0.0075; // Relaxation parameter

	float C = density / REST_DENSITY - 1;
	vec3 gradC_i = vec3(0.0);

	float lambda = 0;

	if (C != 0.0)
	{
		// Compute gradients dC/dx_j 
		float sum_grad_C2 = 0.0;
		

		for (uint j = 0; j < numParticles; j++){
			const vec3 gradC_j = - (1.0 / REST_DENSITY) * spikyGradientKernel(pposition - particles[j].position);
			sum_grad_C2 += length(gradC_j)*length(gradC_j);
			gradC_i -= gradC_j;
		}

		sum_grad_C2 -= length(gradC_i) * length(gradC_i);

		// Compute lambda
		lambda = -C / (sum_grad_C2);
	}else lambda = 0.0;

	vec3 positionDelta = vec3(1) * lambda * (1.0/REST_DENSITY);

	return positionDelta;
}



// Function to handle boundary collisions
vec3 applyBoundary(vec3 position)
{
	vec3 correctedPosition = position;
	for (int i = 0; i < 3; i++)
	{
		if (correctedPosition[i] < boundaryMin[i])
		{
			correctedPosition[i] = boundaryMin[i];

		}
		else if (correctedPosition[i] > boundaryMax[i])
		{
			correctedPosition[i] = boundaryMax[i];
		}
	}

	return correctedPosition;
}


vec3 XPBDPredict(vec3 position, vec3 exForce, float mass, float deltaTime) {
	return position + deltaTime * deltaTime * (vec3(0, 0, -9.81) + exForce / mass);
}

vec3 XPBDSolver(uint gid, vec3 pposition) {
	vec3 positionDelta = vec3(0);

	//TODO : Handle particle-particle collision

	float density = calculateDensity(gid, pposition);
	positionDelta += applyDensityConstraints(gid, pposition, density);

	

	return positionDelta;
}

void XPBDIntegrate(uint gid, vec3 newPos, float deltaTime) {
	Particle p = particles[gid];

	// Update particle positions
	p.position = newPos;

	// Update particle velocities
	p.velocity = (p.position - particles[gid].position) / deltaTime;

	// Update particle
	particles[gid] = p;
}

// XPBD_Main
void main() {
    uint gid = gl_GlobalInvocationID.x;
    if (gid >= numParticles) return;
	Particle p = particles[gid];

    //Calculate external force
	vec3 externalForce = vec3(0);

	//Predict position
	vec3 newPosition = XPBDPredict(p.position, externalForce, p.mass, dt * speed);

	//Confine in boundary
	newPosition = applyBoundary(newPosition);

	//TODO Neighbor search

    //Correct position by solving constraint
	for (int i = 0; i < 10; i++) {
		newPosition += XPBDSolver(gid, newPosition);
	}

	// Update particle positions and velocities using XPBD
	XPBDIntegrate(gid, newPosition, dt * speed);
}
