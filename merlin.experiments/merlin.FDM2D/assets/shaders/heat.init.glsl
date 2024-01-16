#version 450
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Node {
	double temperature;
	vec3 position;
};

layout (std430, binding = 1) buffer NodeBuffer {
  Node nodes[];
};

uniform uint nodeCount;
uniform uint sqNodeCount;

void main() {
  uint index = gl_GlobalInvocationID.x;
  // Calculate the indices for each dimension using div and mod
  
  Node n = nodes[index];

  n.temperature = 20.0;

  ivec2 c = ivec2( index % sqNodeCount, index / sqNodeCount);

  if(c.y == 0){
	n.temperature = 20.0;
  }
  if(c.y == sqNodeCount-1){
	n.temperature = 60.0;
  }

  nodes[index] = n;
}