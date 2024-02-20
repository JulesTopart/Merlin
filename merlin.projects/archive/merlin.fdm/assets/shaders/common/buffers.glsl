struct Particle {
	vec4 initial_position;	// initial position    x0
	vec4 position;			// current position    x
	vec4 new_position;		// predicted position  x*
	vec4 velocity;			// velocity			   u
	vec4 new_velocity;		// velocity			   u*
	vec4 acceleration;		// acceleration		   a
	float mass;				// mass				   m   (or pseudo mass for boundary particles)
	float density;			// density			   rho
	float temperature;		// temperature		   T
	float new_temperature;	// temperature		   T+
	float pressure;			// pressure			   P
	float lambda;			// lagrange multiplier lambda
	uint phase;				// phase (liquid, solid...)
	uint newIndex;			// sorted indexy
	uint binIndex;			// bin index
};

layout(std430, binding = 0) buffer ParticleBuffer {
	Particle particles[];
};

struct Bin {
	uint count;
	uint sum;
	uint csum;
	uint index;
};

layout(std430, binding = 1) buffer BinBuffer {
	Bin bins[];
};

struct ColorScale {
	int minValue;
	int maxValue;
};

layout(std430, binding = 2) buffer ColorScaleBuffer {
	ColorScale colorScale[];
};

#define LAMBDA_FIELD 0
#define DENSITY_FIELD 1
#define TEMPERATURE_FIELD 2
#define VELOCITY_FIELD 3
#define MAX_FIELD 3

void updateMinMax(int field, float value){
	if(field > MAX_FIELD) return;
	atomicMax(colorScale[field].maxValue, int(value*100.0));
	atomicMin(colorScale[field].minValue, int(value*100));
}

float map(float value, ColorScale range){
	return (value*100.0 - (float(range.minValue)))/(float(range.maxValue - range.minValue));
}

uvec3 getBinCoord(vec4 position) {
	position.xyz -= boundaryMin;
	uvec3 bin3D = uvec3(position / binSize);
	bin3D.x = max( min(bin3D.x, binMax.x -1), 0);
	bin3D.y = max( min(bin3D.y, binMax.y -1), 0);
	bin3D.z = max( min(bin3D.z, binMax.z -1), 0);
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

#define OVERNNS {uvec3 binIndexVec = getBinCoordFromIndex(particles[i].binIndex); \
for (int z = int(binIndexVec.z) - 1; z <= int(binIndexVec.z) + 1; z++) { \
	for (int y = int(binIndexVec.y) - 1; y <= int(binIndexVec.y) + 1; y++) { \
		for (int x = int(binIndexVec.x) - 1; x <= int(binIndexVec.x) + 1; x++) { \
			if (x < 0 || y < 0 || z < 0   ||    x >= binMax.x || y >= binMax.y || z >= binMax.z) continue; \
			uint cindex = getBinIndexFromCoord(uvec3(x, y, z)); \
			if (bins[cindex].count == 0) continue; \
			uint ns = bins[cindex].count; \
			uint st = bins[cindex].sum - bins[cindex].count; \
			for (uint k = st; k < st+min(ns, MAXNN); k++) { \
				uint j = particles[k].newIndex; \
				if (j == i || j >= numParticles) continue;


#define OVERNNS_END }}}}}

#define OVERNNS_I {uvec3 binIndexVec = getBinCoordFromIndex(particles[i].binIndex); \
for (int z = int(binIndexVec.z) - 1; z <= int(binIndexVec.z) + 1; z++) { \
	for (int y = int(binIndexVec.y) - 1; y <= int(binIndexVec.y) + 1; y++) { \
		for (int x = int(binIndexVec.x) - 1; x <= int(binIndexVec.x) + 1; x++) { \
			if (x < 0 || y < 0 || z < 0   ||    x >= binMax.x || y >= binMax.y || z >= binMax.z) continue; \
			uint cindex = getBinIndexFromCoord(uvec3(x, y, z)); \
			if (bins[cindex].count == 0) continue; \
			uint ns = bins[cindex].count; \
			uint st = bins[cindex].sum - bins[cindex].count; \
			for (uint k = st; k < st+min(ns, MAXNN); k++) { \
				uint j = particles[k].newIndex; \
				if (j >= numParticles) continue;