#version 450

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Node {
	double temperature;
    vec3 position;
};

layout (std430, binding = 1) buffer NodeBuffer {
  Node nodes[];
};

uniform uint nodeCount;
uniform uint sqNodeCount;
uniform double dt;

#define M_PI 3.141592653589793
#define H 0.5f           // kernel radius
#define H2 H * H        // radius^2 for optimization

// Smoothing kernel function (Quntic kernel)
double kernel(dvec3 r) {
    double R = length(r);
    double q = R / H;
    double a = (21.0/16.0*M_PI*H*H*H);
    if (q >= 0 && q <= 2) {
        return a * (1.0-pow(float(q/2.0), 4.0))*(2.0*q+1.0);
    }else return 0.0f;
}

double computeHeatFlux(Node p, Node pj) {
    dvec3 r = pj.position - p.position;
    if (length(r) > H) return 0.0;
    double L = length(r);
    double r2 = L*L;
    double A = pow(H, 2) * M_PI; // Cross-sectional area
     // Distance between particles
    double Q = -p.conductivity * A * (pj.temperature - p.temperature)/L;
    return Q;
}


double getTemp(ivec2 p, ivec2 offset){
	p += offset;
	uint ni = p.y * sqNodeCount + p.x;
	return nodes[ni].temperature;
}

void main() {
  uint index = gl_GlobalInvocationID.x;
  ivec2 p = ivec2( index % sqNodeCount, index / sqNodeCount); //xy position in the nodes

  Node n = nodes[index];
  float w = 1;
  
  double t00 = getTemp(p, ivec2( 0, 0));
  double t10 = getTemp(p, ivec2( 1, 0));
  double t01 = getTemp(p, ivec2( 0, 1));
  double t20 = getTemp(p, ivec2(-1, 0));
  double t02 = getTemp(p, ivec2( 0,-1));

  n.temperature = (1.0-w)*t00;
  n.temperature += w * ((1.0 / 4.0) * (t10 + t20 + t01 + t02));
  
  if(p.y == 0){
	n.temperature = 0.0;
  }
  if(p.y == sqNodeCount-1){
	n.temperature = 60.0;
  }

  nodes[index] = n;

}