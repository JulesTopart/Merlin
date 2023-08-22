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
uniform double dt;


double getTemp(ivec2 p, ivec2 offset){
	p += offset;
	uint ni = p.y * sqNodeCount + p.x;
	return nodes[ni].temperature;
}

void main() {
  uint index = gl_GlobalInvocationID.x;
  ivec2 p = ivec2( index % sqNodeCount, index / sqNodeCount); //xy position in the nodes

  Node n = nodes[index];
  float w = 1;
  
  double t00 = getTemp(p, ivec2( 0, 0));
  double t10 = getTemp(p, ivec2( 1, 0));
  double t01 = getTemp(p, ivec2( 0, 1));
  double t20 = getTemp(p, ivec2(-1, 0));
  double t02 = getTemp(p, ivec2( 0,-1));

  n.temperature = (1.0-w)*t00;
  n.temperature += w * ((1.0 / 4.0) * (t10 + t20 + t01 + t02));
  
  if(p.y == 0){
	n.temperature = 20.0;
  }
  if(p.y == sqNodeCount-1){
	n.temperature = 60.0;
  }

  nodes[index] = n;

}