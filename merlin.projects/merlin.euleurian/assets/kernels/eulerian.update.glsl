#version 460

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

struct FieldProperty {
	vec3 u;		//Velocity
	float p;	//Pressure

	float d;	//Divergence
	float m;	//mass
	float t;	//Temperature
	float s;	//Grid element type (solid, liquid...)
};

layout(std430, binding = 1) buffer FieldBuffer {
	FieldProperty field[];
};

layout(std430, binding = 2) buffer NewFieldBuffer {
	FieldProperty newField[];
};

#define DENSITY 1000.0f
#define OVERRELAXATION 1.9f

uniform float h; //grid spacing
uniform float speed; //grid spacing
uniform float dt; //timestep
uniform uint grid; //grid 

uint getIndex(uint x, uint y, uint z, uint gridSize) {
	return x + y * gridSize + z * gridSize * gridSize;
}

void integrate(uint gid, float dt) {
	newField[gid].u += vec3(0, 0, 9.81) * dt;
}

// Function to get the velocity at a given position using trilinear interpolation
vec3 getVelocity(vec3 position, uint gridSize, float gridSpacing) {
	// Calculate the grid cell indices
	uint x = uint(position.x / gridSpacing);
	uint y = uint(position.y / gridSpacing);
	uint z = uint(position.z / gridSpacing);

	// Calculate the fractional offsets within the cell
	float fx = (position.x - float(x) * gridSpacing) / gridSpacing;
	float fy = (position.y - float(y) * gridSpacing) / gridSpacing;
	float fz = (position.z - float(z) * gridSpacing) / gridSpacing;

	// Get the velocities at the corners of the cell
	vec3 v000 = field[getIndex(x, y, z, gridSize)].u;
	vec3 v100 = field[getIndex(x + 1, y, z, gridSize)].u;
	vec3 v010 = field[getIndex(x, y + 1, z, gridSize)].u;
	vec3 v001 = field[getIndex(x, y, z + 1, gridSize)].u;
	vec3 v110 = field[getIndex(x + 1, y + 1, z, gridSize)].u;
	vec3 v101 = field[getIndex(x + 1, y, z + 1, gridSize)].u;
	vec3 v011 = field[getIndex(x, y + 1, z + 1, gridSize)].u;
	vec3 v111 = field[getIndex(x + 1, y + 1, z + 1, gridSize)].u;

	// Perform trilinear interpolation
	vec3 v = mix(mix(mix(v000, v100, fx), mix(v010, v110, fx), fy), mix(mix(v001, v101, fx), mix(v011, v111, fx), fy), fz);
	v = field[getIndex(x, y, z, gridSize)].u;
	return v;
}

void advection(uint gid, float time, uint gridSize, float gridSpacing) {
	// Get the current position
	float x = mod(gid, gridSize);
	float y = mod(gid / gridSize, gridSize) ;
	float z = gid / (gridSize * gridSize);

	vec3 currentPosition = vec3(float(x) * gridSpacing, float(y) * gridSpacing, float(z) * gridSpacing);

	// Calculate the velocity at the current position
	vec3 velocity = getVelocity(currentPosition, gridSize, gridSpacing);

	// Calculate the backward Euler integration
	vec3 newPosition = currentPosition - time * velocity;

	// Clamp the new position to the grid boundaries
	newPosition = clamp(newPosition, vec3(0), vec3(float(gridSize - 1) * gridSpacing));

	// Calculate the new grid cell indices
	uint newX = uint(newPosition.x / gridSpacing);
	uint newY = uint(newPosition.y / gridSpacing);
	uint newZ = uint(newPosition.z / gridSpacing);

	// Calculate the fractional offsets within the new cell
	float offsetX = (newPosition.x - float(newX) * gridSpacing) / gridSpacing;
	float offsetY = (newPosition.y - float(newY) * gridSpacing) / gridSpacing;
	float offsetZ = (newPosition.z - float(newZ) * gridSpacing) / gridSpacing;

	// Get the values at the corners of the new cell
	FieldProperty p000 = field[getIndex(newX, newY, newZ, gridSize)];
	FieldProperty p100 = field[getIndex(newX + 1, newY, newZ, gridSize)];
	FieldProperty p010 = field[getIndex(newX, newY + 1, newZ, gridSize)];
	FieldProperty p001 = field[getIndex(newX, newY, newZ + 1, gridSize)];
	FieldProperty p110 = field[getIndex(newX + 1, newY + 1, newZ, gridSize)];
	FieldProperty p101 = field[getIndex(newX + 1, newY, newZ + 1, gridSize)];
	FieldProperty p011 = field[getIndex(newX, newY + 1, newZ + 1, gridSize)];
	FieldProperty p111 = field[getIndex(newX + 1, newY + 1, newZ + 1, gridSize)];

	// Perform trilinear interpolation for each field property
	newField[gid].u = mix(mix(mix(p000.u, p100.u, offsetX), mix(p010.u, p110.u, offsetX), offsetY),
		mix(mix(p001.u, p101.u, offsetX), mix(p011.u, p111.u, offsetX), offsetY), offsetZ);
	newField[gid].p = mix(mix(mix(p000.p, p100.p, offsetX), mix(p010.p, p110.p, offsetX), offsetY),
		mix(mix(p001.p, p101.p, offsetX), mix(p011.p, p111.p, offsetX), offsetY), offsetZ);
	//newField[gid].d = mix(mix(mix(p000.d, p100.d, offsetX), mix(p010.d, p110.d, offsetX), offsetY),
		//mix(mix(p001.d, p101.d, offsetX), mix(p011.d, p111.d, offsetX), offsetY), offsetZ);
	//newField[gid].m = mix(mix(mix(p000.m, p100.m, offsetX), mix(p010.m, p110.m, offsetX), offsetY),
		//mix(mix(p001.m, p101.m, offsetX), mix(p011.m, p111.m, offsetX), offsetY), offsetZ);
	//newField[gid].t = mix(mix(mix(p000.t, p100.t, offsetX), mix(p010.t, p110.t, offsetX), offsetY),
		//mix(mix(p001.t, p101.t, offsetX), mix(p011.t, p111.t, offsetX), offsetY), offsetZ);
	//newField[gid].s = mix(mix(mix(p000.s, p100.s, offsetX), mix(p010.s, p110.s, offsetX), offsetY),
		//mix(mix(p001.s, p101.s, offsetX), mix(p011.s, p111.s, offsetX), offsetY), offsetZ);

}

void solvePressure(uint gid, uint gridSize, float gridSpacing) {
	// Get the current position
	uint x = gid % gridSize;
	uint y = (gid / gridSize) % gridSize;
	uint z = gid / (gridSize * gridSize);
	vec3 currentPosition = vec3(float(x) * gridSpacing, float(y) * gridSpacing, float(z) * gridSpacing);

	// Calculate the pressure at the current position
	float pressure = field[gid].p;

	// Calculate the indices of the neighboring cells
	uint leftIndex = (x > 0) ? getIndex(x - 1, y, z, gridSize) : gid;
	uint rightIndex = (x < gridSize - 1) ? getIndex(x + 1, y, z, gridSize) : gid;
	uint bottomIndex = (y > 0) ? getIndex(x, y - 1, z, gridSize) : gid;
	uint topIndex = (y < gridSize - 1) ? getIndex(x, y + 1, z, gridSize) : gid;
	uint frontIndex = (z > 0) ? getIndex(x, y, z - 1, gridSize) : gid;
	uint backIndex = (z < gridSize - 1) ? getIndex(x, y, z + 1, gridSize) : gid;

	// Calculate the pressure gradients in the neighboring cells
	float pressureGradientX = (field[rightIndex].p - pressure) / gridSpacing;
	float pressureGradientY = (field[topIndex].p - pressure) / gridSpacing;
	float pressureGradientZ = (field[backIndex].p - pressure) / gridSpacing;

	// Calculate the new pressure using the Poisson equation
	float newPressure = (pressureGradientX + pressureGradientY + pressureGradientZ - field[gid].d) / 6.0f;

	// Store the new pressure in the new field
	newField[gid].p = mix(field[gid].p, newPressure, OVERRELAXATION);
}

void main() {
	uint gid = gl_GlobalInvocationID.x;

	uint ix = gid % grid;
	uint iy = (gid / grid) % grid;
	uint iz = gid / (grid * grid);

	if (gid >= grid * grid * grid) return;
	field[gid] = newField[gid];

	float timeStep = 0.016 * speed;

	if (iy < 8) { //Y == 0

		if (ix > grid/2 - 8 && ix < grid/2 + 8 && iz > grid/2 - 8 && iz < grid/2 + 8) {
			field[gid].u = vec3(0, 0.1, 0);
			field[gid].s = 0;
			field[gid].p = 1;
			field[gid].t = 25;
		}

	}

	if (iz == 1) { //z == 0

		if (ix > grid / 2 - 8 && ix < grid / 2 + 8 && iy > grid / 2 - 8 && iy < grid / 2 + 8) {
			field[gid].u = vec3(0, 0,0.1);
			field[gid].s = 0;
			field[gid].p = 1;
			field[gid].t = 25;
		}

	}

	//integrate(gid, timeStep );
	//advection(gid, timeStep, grid, h);

	solvePressure(gid, grid, h);
}
