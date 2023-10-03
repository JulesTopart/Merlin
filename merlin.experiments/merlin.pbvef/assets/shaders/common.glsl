
struct Particle {
	vec4 initial_position; // initial position (x, y, z)
	vec4 position;      // current position    (x, y, z)
	vec4 new_position;  // predicted position  (x, y, z);
	vec4 velocity;		// velocity			   (vx, vy, vz);
	float mass;
	float density;
	float temperature;
	float lambda;
	uint phase;			// phase (liquid, solid...)
	uint newIndex;		// sorted indexy
	uint binIndex;		// bin index
	int padding;		// bin index
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


uvec3 getBinCoord(vec4 position) {
	position *= scale;
	position.xyz -= boundaryMin;
	uvec3 bin3D = uvec3(position / binSize);
	bin3D.x = max(        min(bin3D.x, binMax.x - 1), 0);
	bin3D.y = max(        min(bin3D.y, binMax.y - 1), 0);
	bin3D.z = max(        min(bin3D.z, binMax.z - 1), 0);
	return bin3D;
}

uint getBinIndexFromCoord(uvec3 coord) {
	return (coord.z * binMax.x * binMax.y) + (coord.y * binMax.x) + coord.x;
}

uint getBinIndex(vec4 position) {
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

#define OVERNNS uvec3 binIndexVec = getBinCoordFromIndex(particles[gid].binIndex); \
for (int z = int(binIndexVec.z) - 1; z <= int(binIndexVec.z) + 1; z++) { \
	for (int y = int(binIndexVec.y) - 1; y <= int(binIndexVec.y) + 1; y++) { \
		for (int x = int(binIndexVec.x) - 1; x <= int(binIndexVec.x) + 1; x++) { \
			if (x < 0 || y < 0 || z < 0) continue; \
			if (x >= binMax.x || y >= binMax.y || z >= binMax.z) continue; \
			if (bins[getBinIndexFromCoord(uvec3(x,y,z))].count == 0) continue; \
			uint ns = bins[getBinIndexFromCoord(uvec3(x,y,z))].count; \
			uint st = bins[getBinIndexFromCoord(uvec3(x,y,z))].index; \
			for (uint k = st; k < st+min(ns, MAXNN); k++) { \
				uint j = particles[k].newIndex; \
				if (j == gid || j >= numParticles) continue;


#define OVERNNS_END }}}}