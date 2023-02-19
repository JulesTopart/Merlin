#version 450

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Node {
    vec3 U;		//Position
	vec3 V;		//Velocity

	int enable;	//particle is deactivated
	uint index;
};

layout (std430, binding = 1) buffer NodeBuffer {
  Node nodes[];
};

uniform uint nodeCount;
uniform uint sqNodeCount;

void main() {
  uint index = gl_GlobalInvocationID.x;
  ivec2 p = ivec2( index % sqNodeCount, index / sqNodeCount); //xy position in the nodes

  Node n = nodes[index];




  nodes[index] = n;

}