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

uvec3 getBinCoord(vec3 position) {
	position *= scale;
	position -= boundaryMin;
	uvec3 bin3D = uvec3(position / binSize);
	bin3D.x = max(        min(bin3D.x, binMax.x - 1), 0);
	bin3D.y = max(        min(bin3D.y, binMax.y - 1), 0);
	bin3D.z = max(        min(bin3D.z, binMax.z - 1), 0);
	return bin3D;
}

uint getBinIndexFromCoord(uvec3 coord) {
	return (coord.z * binMax.x * binMax.y) + (coord.y * binMax.x) + coord.x;
}

uint getBinIndex(vec3 position) {
	uvec3 bin3D = getBinCoord(position);
    return getBinIndexFromCoord(bin3D);
}

uvec3 getBinCoordFromIndex(uint index) {
	uint z = index / (binMax.x * binMax.y);
	index -= (z * binMax.x * binMax.y);
	uint y = index / binMax.x;
	uint x = index % binMax.x;
	return uvec3(x, y, z);
}

