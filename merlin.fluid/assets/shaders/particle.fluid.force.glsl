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

uniform float tstep;
uniform float simSpeed;
uniform int count;

#define G vec3(0,0,-9.81f)
#define EPS H // boundary epsilon
#define BOUND_DAMPING -0.8f

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

float spikyKernel(vec3 r) {
    if (length(r) >= H) {
        return 0.0;
    }
    return 15.0/(M_PI * pow(H, 6)) * pow(H-length(r), 3);
}




void main() {
  uint index = gl_GlobalInvocationID.x;
  Particle p = particles[index];
  float dt = tstep * simSpeed;

  float VISC_LAP  = 40.f / (M_PI * pow(H, 5.f)); //Laplacian

    // Compute force
    vec3 fpressure = vec3(0.0);
    vec3 fvisco = vec3(0.0);

    for (uint i = 0; i < count; i++) {
        if (index == i) continue;
        Particle pj = particles[i];
        vec3 r = vec3(particles[i].position) - vec3(p.position);
        if (length(r) < H){
            // compute pressure force contribution
            fpressure += -normalize(r) * MASS * (vec3(p.position) + vec3(pj.position)) / (2.f * pj.density) * spikyKernel(r) * pow(H - length(r), 3.f);
            // compute viscosity force contribution
            fvisco += VISC * MASS * (vec3(pj.velocity) - vec3(p.velocity)) / pj.density * VISC_LAP * (H - r);
        }
    }

    vec3 fgrav = (G * MASS / p.density);
    
    vec3 f = fpressure + fvisco + fgrav;


    if(length(p.velocity + f/p.density * dt) <= 1.0f)
        p.velocity += f/p.density * dt;
    p.position += p.velocity * dt;
  
    // enforce boundary conditions
    //X Boundaries
    if (p.position.x < 0.f)
    {
        p.velocity.x *= BOUND_DAMPING;
        p.position.x = 0.f;
    }
    if (p.position.x > 4.f)
    {
        p.velocity.x *= BOUND_DAMPING;
        p.position.x = 4.f;
    }

    //Y Boundaries
    if (p.position.y < 0.f)
    {
        p.velocity.y *= BOUND_DAMPING;
        p.position.y = 0.f;
    }
    if (p.position.y > 4.f)
    {
        p.velocity.y *= BOUND_DAMPING;
        p.position.y = 4.f;
    }
    
    //Z Boundaries
    if (p.position.z < 0.f)
    {
        p.velocity.z *= BOUND_DAMPING;
        p.position.z = 0.f;
    }
    if (p.position.z > 4.f)
    {
        p.velocity.z *= BOUND_DAMPING;
        p.position.z = 4.f;
    }
    

  particles[index] = p;
}