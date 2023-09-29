struct Particle {
	vec3 position;      // current position   (x, y, z, 0)
	float density;
	vec3 new_position;  // predicted position (x, y, z, 0);
	float lambda;
	vec3 velocity;		// velocity			  (vx, vy, vz, 0);
	float temperature;  // temperature
	float a;			// a
	float b;			// b
	uint newIndex;	// sorted index
	uint binIndex;		// spatial hashing
};

layout(std430) buffer ParticleBuffer {
	Particle particles[];
};

struct Bin {
	uint count;
	uint sum;
	uint csum;
	uint index;
};

layout(std430) buffer BinBuffer {
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

#define OVERNNS uvec3 binIndexVec = getBinCoord(particles[gid].position); \
for (uint z = binIndexVec.z - (binIndexVec.z > 0 ? 1 : 0); z <= binIndexVec.z + 1; z++) { \
	for (uint y = binIndexVec.y - (binIndexVec.y > 0 ? 1 : 0); y <= binIndexVec.y + 1; y++) { \
		for (uint x = binIndexVec.x - (binIndexVec.x > 0 ? 1 : 0); x <= binIndexVec.x + 1; x++) { \
			if (x < 0 || y < 0 || z < 0) continue; \
			if (x > binResolution || y > binResolution || z > binResolution) continue; \
			uint ns = bins[particles[gid].binIndex].count; \
			uint st = bins[particles[gid].binIndex].sum - ns; \
			for (uint k = st; k < st+min(ns, 32); k++) { \
				uint j = particles[k].newIndex; \
				if (j == gid || j > numParticles) continue;


#define OVERNNS_END }}}}