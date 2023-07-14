#version 460
#pragma shader_stage(compute)

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

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


// Constants
#define GRID_SIZE 16
#define CELL_SIZE (1.0f / float(GRID_SIZE))
#define HASH_PRIME1 73856093
#define HASH_PRIME2 19349663
#define HASH_PRIME3 83492791


// SPH Parameters
//const float H = 0.045; // Kernel radius
const float H = 4.2 * 0.03125; // Kernel radius // 2.4*0.375
const float H2 = H * H;
const float REST_DENSITY = 1000.0; //1000.0
const float STIFFNESS = 10.0; //70000.0
const float VISCOSITY = 0.0; //1000.0
const float maxVelocity = 100.4 * 0.03125; //1000.0

// Kernel Functions Precomputed Constants
const float POLY6_COEFFICIENT = 315.0 / (64.0 * 3.14159265359 * pow(H, 9));
const float SPIKY_GRAD_COEFFICIENT = -45.0 / (3.14159265359 * pow(H, 6));
const float VISC_LAPLACE_COEFFICIENT = 45.0 / (3.14159265359 * pow(H, 6));

// Define boundary
const vec3 boundaryMin = vec3(0.0, 0.0, 0);
const vec3 boundaryMax = vec3(2, 1.5, 1.5);
const float boundaryRestitution = 0.005; // Bounce factor
const float boundaryRepulsionDistance = 0.03125;
const float boundaryRepulsionForce = 1;

uniform float dt;
uniform float speed;
uniform int numParticles;


/*
void main() {
  uint index = gl_GlobalInvocationID.x;

  vec3 accel;
  if(particles[index].position.z > 0) accel = vec3(0, 0, -9.81) * 0.001f;
  else{
	  particles[index].velocity = vec4(0,0,0,0);
	  accel = vec3(0);
  }
  particles[index].position += particles[index].velocity * float(dt) * speed;
  particles[index].velocity += vec4(accel * float(dt) * speed, particles[index].position.w);

}
*/

// Define a hash function
uint hashFunction(ivec3 cell)
{
	return (cell.x * HASH_PRIME1) ^ (cell.y * HASH_PRIME2) ^ (cell.z * HASH_PRIME3);
}

// Calculate the cell position for a given particle position
ivec3 calculateCellPosition(vec3 position)
{
	return ivec3(floor(position / CELL_SIZE));
}


// SPH_Kernels
float poly6Kernel(float rSquared)
{
	if (rSquared <= H2)
	{
		return POLY6_COEFFICIENT * pow(H2 - rSquared, 3);
	}
	return 0.0;
}

vec3 spikyGradientKernel(vec3 r, float rLength)
{
	if (rLength <= H && rLength > 0.0)
	{
		float spikyGrad = SPIKY_GRAD_COEFFICIENT * pow(H - rLength, 2);
		return (spikyGrad / rLength) * r;
	}
	return vec3(0.0);
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
void updateDensity(uint gid)
{
	float density = 0.0;
	Particle p = particles[gid];

	for (uint j = 0; j < numParticles; j++) {
		if (j == gid) continue;

		Particle pj = particles[j];
		vec3 diff = p.position - pj.position;
		float rSquared = dot(diff, diff);

		density += pj.mass * poly6Kernel(rSquared);
	}

	particles[gid].density = max(density, 0.1); //Update density
}



// SPH_Viscosity
void updateDensityGrid(uint gid)
{
	vec3 force = vec3(0.0);
	Particle p = particles[gid];

	ivec3 cell = calculateCellPosition(p.position);

	float density = 0.0;

	for (int z = -1; z <= 1; ++z)
	{
		for (int y = -1; y <= 1; ++y)
		{
			for (int x = -1; x <= 1; ++x)
			{
				ivec3 neighborCell = cell + ivec3(x, y, z);
				uint neighborHash = hashFunction(neighborCell);

				for (uint j = 0; j < numParticles; j++)
				{
					if (j == gid) continue;
					Particle pj = particles[j];

					// Check if the particle is in the neighboring cell
					if (hashFunction(calculateCellPosition(pj.position)) == neighborHash)
					{
						vec3 diff = p.position - pj.position;
						float rSquared = dot(diff, diff);

						density += pj.mass * poly6Kernel(rSquared);
					}
				}
			}
		}
	}

	particles[gid].density = max(density, 1); //Update density
}


// SPH_Pressure
vec3 calculatePressureForce(uint gid)
{
	vec3 force = vec3(0.0);
	Particle p = particles[gid];
	float pPressure = STIFFNESS * (p.density - REST_DENSITY);

	for (uint j = 0; j < numParticles; j++)
	{
		if (j == gid) continue;

		Particle pj = particles[j];
		vec3 diff = p.position - pj.position;
		float rSquared = dot(diff, diff);
		float rLength = sqrt(rSquared);
		vec3 spikyGrad = spikyGradientKernel(diff, rLength);

		float pjPressure = STIFFNESS * (pj.density - REST_DENSITY);
		
		force -= (p.mass * pj.mass) * (pPressure / (p.density * p.density) + pjPressure / (pj.density * pj.density)) * spikyGrad;
	}
	particles[gid].pressure = length(force);
	return force;
}


// SPH_Viscosity
vec3 calculatePressureForceGrid(uint gid)
{
	vec3 force = vec3(0.0);
	Particle p = particles[gid];
	float pPressure = STIFFNESS * (p.density - REST_DENSITY);

	ivec3 cell = calculateCellPosition(p.position);

	float density = 0.0;

	for (int z = -1; z <= 1; ++z)
	{
		for (int y = -1; y <= 1; ++y)
		{
			for (int x = -1; x <= 1; ++x)
			{
				ivec3 neighborCell = cell + ivec3(x, y, z);
				uint neighborHash = hashFunction(neighborCell);

				for (uint j = 0; j < numParticles; j++)
				{
					if (j == gid) continue;
					Particle pj = particles[j];

					// Check if the particle is in the neighboring cell
					if (hashFunction(calculateCellPosition(pj.position)) == neighborHash)
					{
						vec3 diff = p.position - pj.position;
						float rSquared = dot(diff, diff);
						float rLength = sqrt(rSquared);
						vec3 spikyGrad = spikyGradientKernel(diff, rLength);

						float pjPressure = STIFFNESS * (pj.density - REST_DENSITY);
						force -= (p.mass * pj.mass) * (pPressure / (p.density * p.density) + pjPressure / (pj.density * pj.density)) * spikyGrad;
					}
				}
			}
		}
	}

	return force;
}


// SPH_Viscoelasticity
vec3 calculateViscoelasticityForce(uint gid, float viscoelasticityCoefficient)
{
	vec3 force = vec3(0.0);
	Particle p = particles[gid];

	for (uint j = 0; j < numParticles; j++)
	{
		if (j == gid) continue;

		Particle pj = particles[j];
		vec3 diff = p.position - pj.position;
		float rSquared = dot(diff, diff);
		float rLength = sqrt(rSquared);
		vec3 spikyGrad = spikyGradientKernel(diff, rLength);

		force += (p.mass * pj.mass) * (spikyGrad / (p.density * pj.density)) * viscoelasticityCoefficient;
	}

	return force;
}



// SPH_Viscosity
vec3 calculateViscosityForce(uint gid)
{
	vec3 force = vec3(0.0);
	Particle p = particles[gid];

	for (uint j = 0; j < numParticles; j++)
	{
		if (j == gid) continue;

		Particle pj = particles[j];
		vec3 diff = p.position - pj.position;
		float rSquared = dot(diff, diff);
		float rLength = sqrt(rSquared);

		float viscosityLaplace = viscosityLaplacianKernel(rLength);
		vec3 velocityDiff = p.velocity - pj.velocity;

		force += (p.mass * pj.mass) * (velocityDiff / (p.density * pj.density)) * VISCOSITY * viscosityLaplace;
	}

	return force;
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
// SPH_Integrate

void integrate(uint gid, vec3 totalForce, float deltaTime)
{
	Particle p = particles[gid];
	vec3 acceleration = totalForce / p.mass;
	acceleration += vec3(0, 0, -9.810);
	// Integrate velocity
	p.velocity += acceleration * deltaTime;

	if (length(p.velocity) > maxVelocity) p.velocity = normalize(p.velocity) * maxVelocity;
	// Integrate position
	p.position += p.velocity * deltaTime;

	// Update particle
	particles[gid] = p;
}



// Function to handle boundary collisions
void handleBoundaryCollision(uint gid)
{
	vec3 position = particles[gid].position;
	vec3 velocity = particles[gid].velocity;

	for (int i = 0; i < 3; i++)
	{
		if (position[i] < boundaryMin[i])
		{
			position[i] = boundaryMin[i];
			velocity[i] = -velocity[i] * 0.005;
		}
		else if (position[i] > boundaryMax[i])
		{
			position[i] = boundaryMax[i];
			velocity[i] = -velocity[i] * 0.005;
		}
	}

	particles[gid].position = position;
	particles[gid].velocity = velocity;
}


void main()
{
	uint gid = gl_GlobalInvocationID.x;
	if (gid >= numParticles)
	{
		return;
	}



	// Update particles' density
	updateDensity(gid);
	//updateDensityGrid(gid);

	// Calculate pressure and viscosity forces
	vec3 pressureForce = calculatePressureForce(gid);//calculatePressureForce(gid);
	//vec3 pressureForce = calculatePressureForceGrid(gid);//calculatePressureForce(gid);
	vec3 viscosityForce = calculateViscosityForce(gid);

	// Calculate viscoelasticity forces
	vec3 viscoelasticityForce = vec3(0);//calculateViscoelasticityForce(gid, 0.8);

	vec3 boundaryForce = calculateBoundaryForce(gid);

	// Combine forces
	vec3 totalForce = pressureForce * 1.0 + viscosityForce * 1.0 + viscoelasticityForce * 0.0 + boundaryForce;

	// Integrate particle positions and velocities
	integrate(gid, totalForce, dt * speed);
	handleBoundaryCollision(gid);

}



/*
void main() {
	uint index = gl_GlobalInvocationID.x;

	// Compute density and pressure forces
	vec3 pressureForce = vec3(0);
	vec3 viscosityForce = vec3(0);
	float density = 0;
	for (int i = 0; i < numParticles; ++i) {
		if (i == index) continue;
		vec3 r = particles[index].position.xyz - particles[i].position.xyz;
		float r2 = dot(r, r);
		if (r2 < h * h) {
			float rlen = sqrt(r2);
			density += particles[i].position.w * Wpoly6(r2);
			vec3 grad = WspikyGrad(r, rlen);
			pressureForce += -grad * particles[i].position.w * k * (particles[index].velocity.w - restDensity) * (particles[i].velocity.w - restDensity);
			viscosityForce += (particles[i].velocity.xyz - particles[index].velocity.xyz) * particles[i].position.w / particles[i].velocity.w;
		}
	}
	density = min(restDensity, density);

	particles[index].velocity.w = density;


	// Integrate forces and update positions
	vec3 accel = vec3(0, 0, -9.81*1000.0) + pressureForce / density + viscosityForce;

	particles[index].velocity += vec4(accel * dt * speed,0);
	particles[index].position += vec4(particles[index].velocity.xyz * dt * speed,0);


	// Floor collision detection and response
	float floorZ = -1.0;  // Set the floor's z-value
		float particleRadius = 0.1; // Set the particle's radius

		if (particles[index].position.z - particleRadius <= floorZ) {
		// Set the particle position to be slightly above the floor to avoid interpenetration
		particles[index].position.z = floorZ + particleRadius;

		// Reflect the velocity and apply a damping factor to simulate energy loss during collision
		float dampingFactor = 0.4; // Set the damping factor (0.4 means 40% of the velocity is retained)
		particles[index].velocity.z = -particles[index].velocity.z * dampingFactor;
	}

		particles[index].position.x = min(particles[index].position.x, 5);
		particles[index].position.y = min(particles[index].position.y, 5);
		particles[index].position.z = min(particles[index].position.z, 5);

		particles[index].position.x = max(particles[index].position.x, 0);
		particles[index].position.y = max(particles[index].position.y, 0);
		particles[index].position.z = max(particles[index].position.z, 0);


}

*/