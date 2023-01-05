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

uniform float tstep;
uniform float simSpeed;
uniform int count;

#define M_PI 3.141592653589793
#define REST_DENS 300.f  // rest density
#define GAS_CONST 2000.f // const for equation of state
#define H 0.2f           // kernel radius
#define H2 H * H        // radius^2 for optimization
#define VISC 200.f       // viscosity constant
#define G 9.81
#define EPS H // boundary epsilon
#define BOUND_DAMPING -0.5f
#define MASS 2.5f        // assume all particles have the same mass

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
  Particle p = Pbuffer.particles[index];
  float dt = tstep * simSpeed;

  float VISC_LAP  = 40.f / (M_PI * pow(H, 5.f)); //Laplacian

    // Compute force
    vec3 fpressure = vec3(0.0);
    vec3 fvisco = vec3(0.0);
    for (uint i = 0; i < count; i++) {
        if (index == i) continue;
        Particle pj = Pbuffer.particles[i];
        vec3 r = vec3(Pbuffer.particles[i].position) - vec3(p.position);
        if (length(r) < H){
            // compute pressure force contribution
            fpressure += -normalize(r) * MASS * (vec3(p.position) + vec3(pj.position)) / (2.f * pj.density) * spikyKernel(r) * pow(H - length(r), 3.f);
            // compute viscosity force contribution
            fvisco += VISC * MASS * (vec3(pj.velocity) - vec3(p.velocity)) / pj.density * VISC_LAP * (H - r);
        }
    }

    vec3 fgrav = (G * MASS / p.density) * vec3(0,0,-1);
    
    vec3 f = fpressure + fvisco + fgrav;

    p.velocity += vec4(f,0.0)/p.density * dt;
    p.position += p.velocity * dt;
  
    // enforce boundary conditions
    //X Boundaries

    /*
    if (p.position.x - EPS < 0.f){
        p.velocity.x *= BOUND_DAMPING;
        p.position.x = EPS;
    }

    if (p.position.x  + EPS > 2.0){
        p.velocity.x *= BOUND_DAMPING;
        p.position.x = 2.0 - EPS;
    }

    //Y Boundaries
    if (p.position.y - EPS < 0.f)
    {
        p.velocity.y *= BOUND_DAMPING;
        p.position.y = EPS;
    }
    if(p.position.y  + EPS > 2.0)
    {
        p.velocity.y *= BOUND_DAMPING;
        p.position.y = 2.0 - EPS;
    }

    //Z Boundaries
    if (p.position.z - EPS < 0.f)
    {
        p.velocity.z *= BOUND_DAMPING;
        p.position.z = EPS;
    }
    if(p.position.z  + EPS > 2.0)
    {
        p.velocity.z *= BOUND_DAMPING;
        p.position.z = 2.0 - EPS;
    }
    */

  Pbuffer.particles[index] = p;
}