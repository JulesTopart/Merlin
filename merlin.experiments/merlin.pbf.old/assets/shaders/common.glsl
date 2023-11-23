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


ivec3 getBinIndex(vec3 position) {
	ivec3 result = ivec3(floor(float(binResolution) * position * scale) / (2.0));
	result.x = max(min(result.x, int(binResolution)), 0);
	result.y = max(min(result.y, int(binResolution)), 0);
	result.z = max(min(result.z, int(binResolution)), 0);
	return result;
}