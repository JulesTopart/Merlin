#version 460

layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

struct Particle {
	dvec3 position;
	dvec3 velocity;
	double temperature;
	double conductivity;
	double capacity;
	double density;
	double pressure;
	double mass;
};

layout (std430, binding = 1) buffer ParticleBuffer {
  Particle particles[];
};


uniform uint grid;
uniform float gridSpacing;


void main() {
  uint index = gl_GlobalInvocationID.x;
  // Calculate the indices for each dimension using div and mod
  uint i = index / (grid * grid);
  uint j = (index / grid) % grid;
  uint k = index % grid;

  Particle pt;

  pt.position = vec3(i + grid/2, j + grid/2, k + grid/2) * gridSpacing;
  pt.velocity = vec3(i,j,k) * 0.00f;
  pt.temperature = 0.0f;
  pt.conductivity = 0.16f;
  pt.capacity = 0.1f;
  pt.density = 0.0f;
  pt.pressure = 0.0f;
  pt.mass = 0.1f;

  particles[index] = pt;
}