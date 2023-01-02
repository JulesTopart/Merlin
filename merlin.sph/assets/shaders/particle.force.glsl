#version 450

layout (local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

#define M_PI 3.1415926535897932384626433832795

struct Particle {
  vec3 position;
  vec3 velocity;
  float density;
  float pressure;
};

layout (std430, binding = 1) buffer ParticleInBuffer {
  Particle particles[];
}inBuffer;

layout (std430, binding = 2) buffer ParticleOutBuffer {
  Particle particles[];
}outBuffer;


//Updated each frame
uniform float tstep;

//Updated once
uniform float smoothingRadius;
uniform uint particleCount;

#define WALL_DAMPING 0.3f
#define GRAVITY_FORCE vec3(0, 0, -9806.65)


float evaluateSmoothingKernel(float r) {
  float sigma = smoothingRadius;
  if (r > sigma) return 0.0f;
  float x = r / sigma;
  return (1.0f / (M_PI * pow(sigma, 3))) * (1.0f - 1.5f * x * x + 0.75f * x * x * x);
}

/*
//Compute attraction between two particle
vec3 computeForces(Particle a, Particle b, float smoothing) {
  float mass = a.mass * b.mass;
  vec3 velocityDifference = b.velocity - a.velocity;
  float pressure = (a.pressure + b.pressure) / 2.0f;
  float viscosity = (a.viscosity + b.viscosity) / 2.0f;
  vec3 force = mass * (pressure / (a.density * a.density) + viscosity * velocityDifference) * smoothing;
  return force;
}
*/

void main() {
  uint index = gl_GlobalInvocationID.x;

  Particle a = inBuffer.particles[index];

  //Calulate forces
  vec3 pforces = vec3(0.0f);
  vec3 vforces = vec3(0.0f);
  for(uint i = 0; i < particleCount; i++){
    if (i == index) continue;
    Particle b = inBuffer.particles[i];
    //float smoothing = evaluateSmoothingKernel(distance(p.position, b.position));
    //forces += computeForces(p, b, smoothing);

    vec3 delta = a.position - b.position;
    float r = length(delta);
    if (r < smoothingRadius)
    {
        pforces -= a.mass * (a.pressure + b.pressure) / (2.f * b.density) *
        // gradient of spiky kernel
            -45.f / (M_PI * pow(smoothingRadius, 6)) * pow(smoothingRadius - r, 2) * normalize(delta);
        vforces += a.mass * (b.velocity - a.velocity) / b.density *
        // Laplacian of viscosity kernel
            45.f / (M_PI * pow(smoothingRadius, 6)) * (smoothingRadius - r);
    }
  }


    vforces *= a.viscosity;
    vec3 eforces = a.density * GRAVITY_FORCE;
    vec3 forces = pforces + vforces + eforces;




  // Update the velocity and position of the particle
  a.velocity += forces * tstep*0.001f / a.density;
  a.position += a.velocity*0.001f * tstep;


      // boundary conditions
    if (a.position.x < -2)
    {
        a.position.x = -2;
        a.velocity.x *= -1 * WALL_DAMPING;
    }
    else if (a.position.x > 2)
    {
        a.position.x = 2;
        a.velocity.x *= -1 * WALL_DAMPING;
    }
    else if (a.position.y < -2)
    {
        a.position.y = -2;
        a.velocity.y *= -1 * WALL_DAMPING;
    }
    else if (a.position.y > 2)
    {
        a.position.y = 2;
        a.velocity.y *= -1 * WALL_DAMPING;
    }
    else if (a.position.z < -1)
    {
        a.position.z = -1;
        a.velocity.z *= -1 * WALL_DAMPING;
    }
    else if (a.position.z > 1)
    {
        a.position.z = 1;
        a.velocity.z *= -1 * WALL_DAMPING;
    }


  outBuffer.particles[index] = a;






}