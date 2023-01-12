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


#define M_PI 3.141592653589793
#define H 0.5f           // kernel radius
#define H2 H * H        // radius^2 for optimization

uniform double tstep;
uniform double simSpeed;
uniform int count;

// Smoothing kernel function (Quntic kernel)
double kernel(dvec3 r) {
    double R = length(r);
    double q = R / H;
    double a = (21.0/16.0*M_PI*H*H*H);
    if (q >= 0 && q <= 2) {
        return a * (1.0-pow(float(q/2.0), 4.0))*(2.0*q+1.0);
    }else return 0.0f;
}

double computeHeatFlux(Particle p, Particle pj) {
    dvec3 r = pj.position - p.position;
    if (length(r) > H) return 0.0;
    double L = length(r);
    double r2 = L*L;
    double A = pow(H, 2) * M_PI; // Cross-sectional area
     // Distance between particles
    double Q = -p.conductivity * A * (pj.temperature - p.temperature)/L;
    return Q;
}


void main() {
    uint index = gl_GlobalInvocationID.x;
    Particle p = particles[index];

    double dt = tstep * simSpeed;

    //Compute density
    double density = 0.0;
    for (uint i = 0; i < count; i++) {
        if (index == i) continue;
        Particle pj = particles[i];
        dvec3 r = pj.position - p.position;
        if (length(r) > H) continue;
        density += pj.mass * length(p.velocity - pj.velocity) * kernel(r);
    }
    p.density = density;


    // Compute temperature
    double flux = 0.0;
    for (uint i = 0; i < count; i++) {
        if (index == i) continue;

        Particle pj = particles[i];
        dvec3 r = pj.position - p.position;

        flux += computeHeatFlux(p, pj) * kernel(r);

    }
    //if(p.density > 0.0) p.temperature += 0.02f / p.density;
    p.temperature += dt * (flux / p.density * p.capacity);

    // Clamp temperature to ambient temperature
    //p.temperature = min(p.temperature, 1.0f);
    p.temperature = p.density;


  if(index == 0) p.temperature = 0.6f;    
  p.velocity += 0.0f * dt;
  p.position += p.velocity * dt;
  

  particles[index] = p;
}