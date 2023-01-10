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
#define H 0.15           // kernel radius
#define H2 H * H        // radius^2 for optimization

uniform double tstep;
uniform float simSpeed;
uniform int count;

// Smoothing kernel function
float poly6Kernel(vec3 r) {
    float R = length(r);
    if(R > H) return 0;

    float R2 = R*R;
    float a = 315.0/(64.0* M_PI * pow(H, 9));
    return a * pow(H2-R2, 3);
    //return a * pow(H2-r2, 3);
}

float computeHeatFlux(Particle p, Particle pj) {
    vec3 r = pj.position - p.position;
    if (length(r) > H) return 0.0;
    float L = length(r);
    float r2 = L*L;
    float A = pow(H, 2) * M_PI; // Cross-sectional area
     // Distance between particles
    float Q = p.conductivity * A * (pj.temperature - p.temperature)/L;
    return Q;
}


void main() {
  uint index = gl_GlobalInvocationID.x;
  Particle p = particles[index];

  double dt = tstep * simSpeed;

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
    double flux = 0.0;
    for (uint i = 0; i < count; i++) {
        if (index == i) continue;

        Particle pj = particles[i];

        vec3 r = pj.position - p.position;
        float r2 = dot(r, r);

        flux += computeHeatFlux(p, pj) * poly6Kernel(r);
        //temperature += pj.mass * computeHeatFlux(p, pj) * poly6Kernel(r);
    }
    //if(p.density > 0.0) flux += 0.000002f / p.density;
    p.temperature = float(dt * (flux / p.mass * p.capacity) + p.temperature);

    // Clamp temperature to ambient temperature
    p.temperature = min(p.temperature, 1.0f);
    //p.temperature = p.density / 20.0;


  if(index == 0) p.temperature = 0.9f;    
  //p.velocity += vec3(0.0,0.0,0.0) * dt;
  //p.position += p.velocity * dt;
  

  particles[index] = p;
}