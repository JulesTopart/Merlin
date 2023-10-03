
uniform float particleRadius = 1.0;
uniform float particleMass = pow(2*particleRadius, 3) * REST_DENSITY; //cube volume density
uniform float smoothingRadius = 4*particleRadius;

uniform float REST_DENSITY = 1000.0;
uniform float dt = 0.0016;
uniform float G = 1000.0*9.81f; //gravity

uniform float speed = 1;
uniform uint numParticles = 0;