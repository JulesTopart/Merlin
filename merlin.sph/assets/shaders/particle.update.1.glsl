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

uniform float tstep;
uniform float simSpeed;
uniform int count;
uniform float smoothing_radius;


// Smoothing kernel function
float w(float r) {
    float h = smoothing_radius;
    float q = r / h;
    if (q > 2.0) {
        return 0.0;
    } else if (q > 1.0) {
        return (3.0 - 2.0 * q) / (2.0 * h);
    } else {
        return (1.0 - 1.5 * q * q + 0.5 * q * q * q) / (h * h * h);
    }
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