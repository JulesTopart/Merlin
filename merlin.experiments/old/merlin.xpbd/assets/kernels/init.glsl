#version 460

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

struct Particle {
	vec3 position;      // position (x, y, z)
	float mass;			// mass(w)

	vec3 velocity;      // velocity (x, y, z) and density (w)
	float density;		// density g/m3

	vec3 stressTensorX; // stress tensor (xx, xy, xz)
	float temperature;	// temperature (K)

	vec3 stressTensorY; // stress tensor (yx, yy, yz, 0)
	float pressure;		// pressure (Pa)

	vec3 stressTensorZ; // stress tensor (zx, zy, zz, 0)
	float strain;		// strain (Pa)
};

layout (std430, binding = 1) buffer ParticleBuffer {
  Particle particles[];
};


uniform uint grid;
uniform float gridSpacing;


void main() {
  uint index = gl_GlobalInvocationID.x;
  // Calculate the indices for each dimension using div and mod
  
  uint i = index / (grid * grid);
  uint j = (index % (grid * grid)) / grid;
  uint k = index % grid;
  

  float space = gridSpacing;

  particles[index].position = vec3(i * space, j * space, k * space + 0.5);
  particles[index].velocity = vec3(0.0,0.0,0.0);

  particles[index].mass = .0000305; //9.5 g
  particles[index].density = 1.0; //1000.0 g/m3
}