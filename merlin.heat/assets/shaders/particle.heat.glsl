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
};


#define M_PI 3.141592653589793
#define H 8.0           // kernel radius
#define H2 H * H        // radius^2 for optimization

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
    if (length(r) > H) return 0.0;
    float r2 = dot(r, r);
    float A = pow(H, 2) * 3.14159; // Cross-sectional area
    float L = sqrt(r2); // Distance between particles
    float Q = -pj.conductivity * A * (pj.temperature - p.temperature) / L;
    return Q * pj.capacity;
}


void main() {
  uint index = gl_GlobalInvocationID.x;
  Particle p = particles[index];

  float dt = tstep * simSpeed;

  //Compute density
  float density = 0.0;
    for (uint i = 0; i < count; i++) {
        if (index == i) continue;
        Particle pj = particles[i];
        vec3 r = pj.position - p.position;
        if (length(r) > H) continue;
        density += pj.mass * poly6Kernel(r);
    }
    p.density = density;


    // Compute temperature
    float temperature = 0.0;
    for (uint i = 0; i < count; i++) {
        if (index == i) continue;

        Particle pj = particles[i];

        vec3 r = pj.position - p.position;
        float r2 = dot(r, r);
        temperature += pj.mass * computeHeatFlux(p, pj) * poly6Kernel(r);
    }
    //temperature += 0.002f / p.density;
    p.temperature = temperature * dt + p.temperature;


    // Clamp temperature to ambient temperature
    p.temperature = max(p.temperature, 1.0f);


  if(index == 0) p.temperature = 0.9f;    
  //p.velocity += vec3(0.0,0.0,0.0) * dt;
  //p.position += p.velocity * dt;
  

  particles[index] = p;
}