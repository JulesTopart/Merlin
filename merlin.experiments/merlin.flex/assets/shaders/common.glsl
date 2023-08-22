struct Particle {
	vec3 position;      // current position   (x, y, z, 0)
	float density;
	vec3 new_position;  // predicted position (x, y, z, 0);
	float lambda;
	vec3 velocity;		// velocity			  (vx, vy, vz, 0);
	float temperature;  // temperature
};

layout(std430, binding = 1) buffer ParticleBuffer {
	Particle particles[];
};


struct Bin {
	uint count;
	uint indices[32];
};


layout(std430, binding = 2) buffer BinBuffer {
	Bin bins[];
};

