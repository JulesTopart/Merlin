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

uniform int count;

#define M_PI 3.141592653589793
#define REST_DENS 300.0  // rest density
#define GAS_CONST 2000.0 // const for equation of state
#define H 8           // kernel radius
#define H2 H * H        // radius^2 for optimization
#define VISC 200.0       // viscosity constant
#define MASS 2.0        // assume all particles have the same mass

// Smoothing kernel function
float poly6Kernel(vec3 r) {

    if(length(r) > H) return 0;

    float r2 = dot(r, r);
    float a = 315.0/(64.0* M_PI * pow(H, 9));
    return a * pow(H2-r2, 3);
}


void main() {
    uint index = gl_GlobalInvocationID.x;
    Particle p = particles[index];

    //Compute density
    float density = 0.0;
    for (uint i = 0; i < count; i++) {
        if (index == i) continue;
        vec3 r = vec3(particles[i].position) -  vec3(p.position);
        density += MASS * poly6Kernel(r);
    }

    p.density = density; //density
    p.pressure = GAS_CONST * (density - REST_DENS);; //Pressure

    particles[index] = p;
}