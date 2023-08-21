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

	uvec3 bin3D = floor(position*scale/domain);

	bin3D = uvec3(max(min(bin3D, binMax-uvec3(1)), 0));
    return (bin3D.z * binMax.x * binMax.y) + (bin3D.y * binMax.x) + bin3D.x;
}

uint getBinIndexFromCoord(uvec3 coord) {
	return (coord.z * binMax.x * binMax.y) + (coord.y * binMax.x) + coord.x;
}

uvec3 getBinCoordFromIndex(uint index) {
	uint z = index / (binMax.x * binMax.y);
	index -= (z * binMax.x * binMax.y);
	uint y = index / binMax.x;
	uint x = index % binMax.x;
	return uvec3(x, y, z);
}

uvec3 getBinCoord(vec3 position) {
	position -= boundaryMin;
	uvec3 result = uvec3(floor(float(binResolution) * position * scale) / (2.0));
	result.x = uint(max(min(result.x, int(binResolution) - 1), 0));
	result.y = uint(max(min(result.y, int(binResolution) - 1), 0));
	result.z = uint(max(min(result.z, int(binResolution) - 1), 0));
	return result;
}