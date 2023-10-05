struct Particle {
	vec4 initial_position;	// initial position    x0
	vec4 position;			// current position    x
	vec4 new_position;		// predicted position  x*
	vec4 velocity;			// velocity			   u
	vec4 acceleration;		// acceleration		   a
	float mass;				// mass				   m   (or pseudo mass for boundary particles)
	float density;			// density			   rho
	float temperature;		// temperature		   T
	float lambda;			// lagrange multiplier lambda
	uint phase;				// phase (liquid, solid...)
	uint newIndex;			// sorted indexy
	uint binIndex;			// bin index
	uint padding;			// bin index
};

//binIndex	1459	unsigned int
/*
+ [1099] {47410 1028 }	FluidParticle
+ [1100] {47410 2000 }	FluidParticle
+ [1101] {53938 1487 }	FluidParticle
+ [1102] {53938 32093 }	FluidParticle
+ [1103] {60466 2546 }	FluidParticle
*/

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

#define OVERNNS uvec3 binIndexVec = getBinCoordFromIndex(particles[i].binIndex); \
for (int z = int(binIndexVec.z) - 1; z <= int(binIndexVec.z) + 1; z++) { \
	for (int y = int(binIndexVec.y) - 1; y <= int(binIndexVec.y) + 1; y++) { \
		for (int x = int(binIndexVec.x) - 1; x <= int(binIndexVec.x) + 1; x++) { \
			if (x < 0 || y < 0 || z < 0) continue; \
			if (x >= binMax.x || y >= binMax.y || z >= binMax.z) continue; \
			uint cindex = getBinIndexFromCoord(uvec3(x, y, z)); \
			if (bins[cindex].count == 0) continue; \
			uint ns = bins[cindex].count; \
			uint st = bins[cindex].index; \
			for (uint k = st; k < st+min(ns, MAXNN); k++) { \
				uint j = particles[k].newIndex; \
				if (j == i || j >= numParticles) continue;


#define OVERNNS_END }}}}