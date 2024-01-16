
uniform float particleRadius = 1.0;//mm
uniform float smoothingRadius; //mm
uniform float REST_DENSITY = 1.0;//g/mm3
uniform float particleMass = 1;//g
uniform float dt = 0.016;//s
uniform float G = 9.81f*20.0;//mm/s^-2
uniform float speed = 1;
uniform float pressureMultiplier = 0.01;
uniform float stiffness = 5000000.0;
uniform float alphaVisco = 0.0001;
uniform uint numParticles = 0;
uniform bool stabilizationPass = false;