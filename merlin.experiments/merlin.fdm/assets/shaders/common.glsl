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


uint getBinIndex(vec3 position) {
	position -= boundaryMin;
	uvec3 result = ivec3(floor(float(binResolution) * position * scale) / (2.0));
	result.x = uint(max(min(result.x, int(binResolution)-1), 0));
	result.y = uint(max(min(result.y, int(binResolution)-1), 0));
	result.z = uint(max(min(result.z, int(binResolution)-1), 0));
    return (result.z * binResolution * binResolution) + (result.y * binResolution) + result.x;
}

uint getBinIndexFromCoord(uvec3 coord) {
	return (coord.z * binResolution * binResolution) + (coord.y * binResolution) + coord.x;
}

uvec3 getBinCoordFromIndex(uint index) {
	uint i = index % binResolution;
	uint j = uint((index % (binResolution * binResolution)) / binResolution);
	uint k = uint(index / (binResolution * binResolution)) % binResolution;

	return uvec3(i,j,k);
}

uvec3 getBinCoord(vec3 position) {
	position -= boundaryMin;
	uvec3 result = uvec3(floor(float(binResolution) * position * scale) / (2.0));
	result.x = uint(max(min(result.x, int(binResolution) - 1), 0));
	result.y = uint(max(min(result.y, int(binResolution) - 1), 0));
	result.z = uint(max(min(result.z, int(binResolution) - 1), 0));
	return result;
}