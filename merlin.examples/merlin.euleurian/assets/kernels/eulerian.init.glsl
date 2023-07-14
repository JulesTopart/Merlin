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

uniform uint grid;

void main() {
	uint index = gl_GlobalInvocationID.x;
	// Calculate the indices for each dimension using div and mod

	float x = mod(index, float(grid)) / float(grid);
	float y = mod(index / float(grid), float(grid)) / float(grid);
	float z = index / (float(grid) * float(grid)) / float(grid);


	field[index].u = vec3(x,y,z);
	field[index].p = 0.0;

	field[index].d = 0; //9.5 g
	field[index].m = 1.0; //9.5 g
	field[index].t = 0; //9.5 g
	field[index].s = 0; //9.5 g

	newField[index] = field[index];
}