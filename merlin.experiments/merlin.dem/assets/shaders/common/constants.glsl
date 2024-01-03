#define BTHREAD 256
#define PTHREAD 512

#define UNUSED 0
#define SOLID 1
#define FLUID 2
#define GAS 3
#define GRANULAR 4
#define BOUNDARY 5

// --- Global ---
const float EPSILON = 1.0e-6f; //Small error epsilon
#define INV_REST_DENSITY (1.0 / REST_DENSITY)

//XPBD
const float relaxation = 0.0033;

// --- Heat Transfer ---
const float ambientTemperature = 298.15;
const float floorTemperature = 333.15;
const float nozzleTemperature = 488.15;

const float kPLA = 1.83e-4;// W / mm.K
const float CpPLA = 4.0; // J / g.K

const float kAmbient = 1.0e-4;
const float kPlateau = 3.0e-4;

const float kWater = 6.06e-4; // W / mm.K
const float CpWater = 4.18; // J /g.K



// --- Artificial Pressure ---
//Artificial Pressure Strength(s_corr) : 0.0001
const float pressureStrength = 35000.0; //K
//Artificial Pressure Radius (delta q): 0.03m
#define pressureRadius 0.3*H // m
///Artificial Pressure Power(n) : 4
const float pressurePower = 7;// m

//Artificial Viscosity(c) : <= 0.01
//const float artificialViscosity = 0.28;
const float artificialViscosity = 0.01;
const float floorFriction = 0.2;


//Rheological model
const float rheo_k = 50.0; // Consistency index
const float rheo_n = 0.5; // Flow behavior index


#define MAXNN 32
// --- Domain ---
const uint binResolution = 128;
const vec3 domain = vec3(30, 30, 100);
//const vec3 domain = vec3(300, 30, 30);
const vec3 boundaryMin = vec3(-domain.x/2.0 , -domain.y/2.0, 0);
const vec3 boundaryMax = vec3(domain.x / 2.0, domain.y / 2.0, domain.z);

const float binSize = max(max(domain.x, domain.y), domain.z) / float(binResolution);
const uvec3 binMax = uvec3(domain / binSize);
const uint binCount = binMax.x * binMax.y * binMax.z;

// Define boundary
const float boundaryDamping = -0.001; // Bounce factor


