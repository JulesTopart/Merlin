#version 460
#pragma shader_stage(compute)

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Particle{
	vec3 position; 	// Position (x, y, z)
	float mass; 	// Mass
	vec3 velocity; 	// Velocity (x, y, z)
	float density; 	// Inverse density
};

layout(std430, binding = 1) buffer ParticleBuffer {
	Particle particles[];
};

// Define boundary
const vec3 boundaryMin = vec3(0.0, 0.0, 0);
const vec3 boundaryMax = vec3(10, 6, 6);
const float boundaryRestitution = 0.50; // Bounce factor
const float collisionRestitution = 1.0; // Bounce factor
const float repulsionStrength = 100.0; // Bounce factor


uniform double dt;
uniform float speed;
uniform int numParticles;


void updateCollision(uint gid) {
	vec3 pos = particles[gid].position;
	vec3 vel = particles[gid].velocity;

	for (uint i = 0; i < numParticles; i++) {
		if (i == gid) continue;  // Skip self-collision

		vec3 otherPos = particles[i].position;
		vec3 dir = otherPos - pos;
		float dist = length(dir);
		float radiusSum = 0.0375 * 2.0;

		if (dist < radiusSum) {
			// Collision detected
			vec3 otherVel = particles[i].velocity;
			vec3 relVel = vel - otherVel;
			vec3 norm = normalize(dir);

			// Calculate impulse force
			float j = -(1.0 + collisionRestitution) * dot(relVel, norm) / (1.0 / particles[gid].mass + 1.0 / particles[i].mass);
			vec3 impulse = j * norm;

			// Update velocities
			vel += impulse / particles[gid].mass;
			particles[i].velocity -= impulse / particles[i].mass;
		}
	}

	particles[gid].velocity = vel;
}

// SPH_Integrate
void integrate(uint gid, vec3 totalForce, float deltaTime)
{
	Particle p = particles[gid];
	vec3 acceleration = totalForce / p.mass;
	acceleration += vec3(0, 0,-9810);
	// Integrate velocity
	p.velocity += acceleration * deltaTime;
	// Integrate position
	p.position += p.velocity * deltaTime;
	updateCollision(gid);
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
			velocity[i] = -velocity[i] * boundaryRestitution;
		}
		else if (position[i] > boundaryMax[i])
		{
			position[i] = boundaryMax[i];
			velocity[i] = -velocity[i] * boundaryRestitution;
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

	// Combine forces
	vec3 totalForce = vec3(0);//calculateCollision(gid);

	// Integrate particle positions and velocities
	integrate(gid, totalForce, float(dt) * speed);
	handleBoundaryCollision(gid);
}
