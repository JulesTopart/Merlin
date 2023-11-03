
uniform float particleRadius = 1.0;//mm
uniform float smoothingRadius; //mm
uniform float REST_DENSITY = 1.0;//g/mm3
uniform float particleMass = 1.0;//g
uniform float dt = 0.00016;//s
uniform float G = 9.81*1000.0f;//mm/s^-2
uniform float speed = 1;
uniform float pressureMultiplier = 0.001;
uniform float alphaVisco = 0.001;
uniform uint numParticles = 0;