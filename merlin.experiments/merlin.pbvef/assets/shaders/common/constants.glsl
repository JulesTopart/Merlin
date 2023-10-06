#define BTHREAD 64
#define PTHREAD 512

#define UNUSED 0
#define SOLID 1
#define FLUID 2
#define GAS 3
#define BOUNDARY 4

// --- Global ---
const float scale = 1;
const float EPSILON = 1.0e-6f; //Small error epsilon
#define INV_REST_DENSITY 1.0 / REST_DENSITY

//XPBD
const float relaxation = 0.0033;

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
const float pressureStrength = 0.1; //K
//Artificial Pressure Radius (delta q): 0.03m
#define pressureRadius 0.3*H // m
///Artificial Pressure Power(n) : 4
const float pressurePower = 4;// m

//Artificial Viscosity(c) : <= 0.01
//const float artificialViscosity = 0.28;
const float artificialViscosity = 0.01;
const float floorFriction = 0.2;


//Rheological model
const float rheo_k = 50.0; // Consistency index
const float rheo_n = 0.5; // Flow behavior index


#define MAXNN 512
// --- Domain ---
const uint binResolution = 64;
const vec3 domain = vec3(100, 40, 100);
const vec3 boundaryMin = vec3(-domain.x/2.0 , -domain.y/2.0, 0);
const vec3 boundaryMax = vec3(domain.x / 2.0, domain.y / 2.0, domain.z);

const float binSize = max(max(domain.x, domain.y), domain.z) / float(binResolution);
const uvec3 binMax = uvec3(domain / binSize);
const uint binCount = binMax.x * binMax.y * binMax.z;

// Define boundary
const float boundaryRestitution = 0.05; // Bounce factor
const float boundaryRepulsionDistance = 0.03125;
const float boundaryRepulsionForce = 0.0;


