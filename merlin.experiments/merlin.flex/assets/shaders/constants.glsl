
/*
Recommended for a traditional dam-break style scenario, similar to the ones in the PBF paper:

Particle mass: 1.0kg
Kernel radius (h): 0.1m
Rest density (rho): 6378.0kg/m^2
Density Iterations: 4
Time step (dt): 0.0083s (2 substeps of a 60hz frame time)
CFM Parameter (epsilon): 600
Artificial Pressure Strength (s_corr): 0.0001
Artificial Pressure Radius (delta q): 0.03m
Artificial Pressure Power (n): 4
Artificial Viscosity (c): <= 0.01"

*/
// --- Global ---
const float scale = 0.025 / (4.0);
const float G = 9.81f; //gravity
const float EPSILON = 1.0e-5f; //Small error epsilon
const float particleMass = 1.0;//kg Mass
const float dt = 0.0016;//s Timestep (16 substeps of a 60hz frame time)
//const float REST_DENSITY = 6378.0; //WATER ?
const float REST_DENSITY = 933.0; //Metled plastic
const float INV_REST_DENSITY = 1.0 / REST_DENSITY;
const float relaxation = 0.0028;
//const float relaxation = 0.0033;
// --- SPH ---
// SPH Parameters
//const float H = 1.2; // Kernel radius // 0.1m
const float H = 1.5; // Kernel radius // 0.1m
const float H2 = H * H;
const float H6 = H * H * H * H * H * H;
const float H9 = H * H * H * H * H * H * H * H * H;
const uint densityIteration = 4;// Incompressibility solving
const float CMF = 600;//CFM Parameter(epsilon) : 

// --- Heat Transfer ---
const float ambientTemperature = 298.15;
const float floorTemperature = 333.15;
const float nozzleTemperature = 488.15;
const float kPLA = 0.183;
const float CpPLA = 400.0;

const float kAmbient = 1.0;
const float kPlateau = 3.0;


// --- Artificial Pressure ---
//Artificial Pressure Strength(s_corr) : 0.0001
const float pressureStrength = 0.00001; //K
//Artificial Pressure Radius (delta q): 0.03m
const float pressureRadius = 0.03;// m
///Artificial Pressure Power(n) : 4
const float pressurePower = 4;// m

//Artificial Viscosity(c) : <= 0.01
const float artificialViscosity = 0.28;
const float floorFriction = 0.8;

// --- Kernels ---
// Kernel Functions Precomputed Constants
const float POLY6_COEFFICIENT = 315.0 / (64.0 * 3.14159265359 * pow(H, 9));
const float SPIKY_GRAD_COEFFICIENT = -45.0 / (3.14159265359 * pow(H, 6));
const float VISC_LAPLACE_COEFFICIENT = 45.0 / (3.14159265359 * pow(H, 6));
#define M_PI 3.14159265358979323846
#define PI_FAC 0.454728408833987

// --- Boundary ---
// Define boundary
const float binsWidth = 2.0; //1m / scale
const vec3 boundaryMin = vec3(0.0, 0.0, 0);
const vec3 boundaryMax = vec3(binsWidth, binsWidth, binsWidth);
const float boundaryRestitution = 0.05; // Bounce factor
const float boundaryRepulsionDistance = 0.03125;
const float boundaryRepulsionForce = 0.0;



// --- Cube of particle ---
const ivec3 resolution = ivec3(boundaryMax/H);

// --- Cube of particle ---
const uint binResolution = 128;
const uint binCount = binResolution * binResolution * binResolution;