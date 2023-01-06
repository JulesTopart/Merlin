#version 460

layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

struct Particle {
    vec3 position;
    vec3 velocity;
    float temperature;
    float conductivity;
    float capacity;
    float density;
    float pressure;
    float mass;
};

layout (std430, binding = 1) buffer ParticleBuffer {
    Particle particles[];
}Pbuffer;


#define M_PI 3.141592653589793
#define REST_DENS 300.0  // rest density
#define GAS_CONST 2000.0 // const for equation of state
#define H 0.2           // kernel radius
#define H2 H * H        // radius^2 for optimization
#define VISC 200.0       // viscosity constant
#define MASS 2.0        // assume all particles have the same mass

uniform float tstep;
uniform float simSpeed;
uniform int count;

// Smoothing kernel function
float poly6Kernel(vec3 r) {

    if(length(r) > H) return 0;

    float r2 = dot(r, r);
    float a = 315.0/(64.0* M_PI * pow(H, 9));
    return a * pow(H2-r2, 3);
}

float computeHeatFlux(Particle p, Particle pj) {
    vec3 r = pj.position - p.position;
    float r2 = dot(r, r);
    if (r2 > smoothing_radius * smoothing_radius) return 0.0;
    float A = pow(smoothing_radius, 2) * 3.14159; // Cross-sectional area
    float L = sqrt(r2); // Distance between particles
    float Q = -pj.conductivity * A * (pj.temperature - p.temperature) / L;
    return Q * pj.capacity;
}


void main() {
  uint index = gl_GlobalInvocationID.x;
  Particle p = Pbuffer.particles[index];

  float dt = tstep * simSpeed;

  //Compute density
  float density = 0.0;
    for (uint i = 0; i < count; i++) {
        if (index == i) continue;
        Particle pj = Pbuffer.particles[i];
        vec3 r = pj.position - p.position;
        float r2 = dot(r, r);
        if (r2 > smoothing_radius * smoothing_radius) continue;
        density += pj.mass * w(sqrt(r2));
    }
    p.density = density;


    // Compute temperature
    float temperature = 0.0;
    for (uint i = 0; i < count; i++) {
        if (index == i) continue;

        Particle pj = Pbuffer.particles[i];

        vec3 r = pj.position - p.position;
        float r2 = dot(r, r);
        temperature += pj.mass * computeHeatFlux(p, pj) * w(sqrt(r2));
    }
    //temperature += 0.002f / p.density;
    p.temperature = temperature * dt + p.temperature;

    // Clamp temperature to ambient temperature
    //p.temperature = max(p.temperature, 0.2f);


  if(index == 0) p.temperature = 0.8f;    
  //p.velocity += vec3(0.0,0.0,0.0) * dt;
  p.position += p.velocity * dt;
  

  Pbuffer.particles[index] = p;
}