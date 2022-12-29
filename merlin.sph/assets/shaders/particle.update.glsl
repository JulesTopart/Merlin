#version 450

layout (local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

struct Particle {
  vec3 position;
  vec3 velocity;
  float mass;
};

layout (std430, binding = 1) buffer ParticleInBuffer {
  Particle particles[];
}inBuffer;

layout (std430, binding = 2) buffer ParticleOutBuffer {
  Particle particles[];
}outBuffer;

uniform float tstep;
uniform float density;
uniform float viscosity;

void main() {
  uint index = gl_GlobalInvocationID.x;
  uint grid = 100;
  // Calculate the indices for each dimension using div and mod
  uint i = index / (grid * grid);
  uint j = (index % (grid * grid)) / grid;
  uint k = index % grid;

  vec3 accel = normalize(vec3(i, j, k))/100.0f;

  outBuffer.particles[index].position = inBuffer.particles[index].position + inBuffer.particles[index].velocity * tstep;
  outBuffer.particles[index].velocity = inBuffer.particles[index].velocity + accel*tstep;

}