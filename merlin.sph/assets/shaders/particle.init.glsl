#version 460

layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

struct Particle {
  vec4 position;
  vec4 velocity;
  vec4 sph; //SPH : Density, Pressure
};

layout (std140, binding = 1) buffer ParticleBuffer {
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

  pt.position = vec4(i + grid/2, j + grid/2, k + grid/2,1) * gridSpacing;
  pt.velocity = vec4(i,j,k,1) * 0.0f;
  pt.sph.x = 0.0f; //Density
  pt.sph.y = 0.0f; //Pressure

  particles[index] = pt;
}