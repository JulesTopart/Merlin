#version 460

layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

struct Particle {
  vec3 position;
  vec3 velocity;
  float temperature;
  float conductivity;
  float capacity;
  float density;
  float mass;
};

layout (std430, binding = 1) buffer ParticleBuffer {
  Particle particles[];
}Pbuffer;

uniform uint grid;
uniform float gridSpacing;
uniform float smoothing_radius;

void main() {
  uint index = gl_GlobalInvocationID.x;
  // Calculate the indices for each dimension using div and mod
  uint i = index / (grid * grid);
  uint j = (index / grid) % grid;
  uint k = index % grid;

  Particle pt;

  pt.position = vec3(i, j, k) * gridSpacing;
  pt.velocity = vec3(i,j,k) * 0.0f;

  pt.temperature = (index == 0) ? 0.6f : 0.2f;
  pt.capacity = 1.0f;
  
  pt.mass = 0.1f;
  pt.density = pt.mass / (4.0 / 3.0 * 3.14159 * pow(smoothing_radius, 3));

  Pbuffer.particles[index] = pt;
}