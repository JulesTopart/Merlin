#version 460

layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

struct Particle {
  vec4 position;
  vec4 velocity;
  float density;
  float pressure;
  float temperature;
  float conductivity;
};

layout (std430, binding = 1) buffer ParticleBuffer {
  Particle particles[];
}Pbuffer;

uniform int count;

#define M_PI 3.141592653589793
#define REST_DENS 300.f  // rest density
#define GAS_CONST 2000.f // const for equation of state
#define H 0.2f           // kernel radius
#define H2 H * H        // radius^2 for optimization
#define VISC 200.f       // viscosity constant
#define MASS 2.5f        // assume all particles have the same mass

// Smoothing kernel function
float poly6Kernel(vec3 r) {

    if(length(r) > H) return 0;

    float r2 = dot(r, r);
    float a = 315.0/(64.0* M_PI * pow(H, 9));
    return a * pow(H2-r2, 3);
}


void main() {
  uint index = gl_GlobalInvocationID.x;
  Particle p = Pbuffer.particles[index];

  //Compute density
  float density = 0.0;
    for (uint i = 0; i < count; i++) {
        if (index == i) continue;
        vec3 r = vec3(Pbuffer.particles[i].position) -  vec3(p.position);
        density += MASS * poly6Kernel(r);
    }
    p.density = density;
    p.pressure = GAS_CONST * (p.density - REST_DENS);

  Pbuffer.particles[index] = p;
}