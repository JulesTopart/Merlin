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

struct ColorScale {
	ivec2 density;
	ivec2 temperature;
	ivec2 position;
	ivec2 velocity;
};

layout(std430) buffer ColorScaleBuffer {
	ColorScale colorScale;
};

#define DENSITY_FIELD 1
#define TEMPERATURE_FIELD 2
#define VELOCITY_FIELD 3

void updateMinMax(int field, in float value){
	switch(field){
	case DENSITY_FIELD : 
		atomicMax(colorScale.density.y, int(value*1000.0));
		atomicMin(colorScale.density.x, int(value*1000.0));
	break;
	case TEMPERATURE_FIELD : 
		atomicMax(colorScale.temperature.y, int(value*1000.0));
		atomicMin(colorScale.temperature.x, int(value*1000.0));
	break;
	case VELOCITY_FIELD : 
		atomicMax(colorScale.velocity.y, int(value*1000.0));
		atomicMin(colorScale.velocity.x, int(value*1000.0));
	break;
	}

}

float map(float value, ivec2 range){
	return (value - (float(range.x)))/(float(range.y - range.x));
}

uvec3 getBinCoord(vec4 position) {
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