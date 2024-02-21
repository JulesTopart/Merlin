#pragma once
#include <Merlin.h>
#include <glm/glm.hpp>


struct Bin {
	GLuint count; //particle count in the bin
	GLuint sum;   //global sum
	GLuint csum;  //local sum
	GLuint index; //bin index
};

struct Particle {
	glm::vec3 position;  //x, y, z
	float density;  //density
	glm::vec3 pposition; //X, Y, Z
	float lambda;  //density
	glm::vec3 velocity;  //vx, vy, vz, padding
	float paddingf;  //density
	GLuint phase;
	GLuint binIndex;
	GLuint id;
	GLuint padding;
};

struct Settings {
	const float particleRadius = 0.5;
	const float smoothingRadius = 4 * particleRadius;
	const float bWidth = smoothingRadius;


	//Boundary Volume dimensions
	glm::vec3 bb = glm::vec3(200, 52, 80);

	// Physics Parameters
	Uniform<float> timestep							= Uniform<float>("dt", 0.0016);
	Uniform<float> restDensity						= Uniform<float>("restDensity", 1.0);
	Uniform<float> particleMass						= Uniform<float>("particleMass", smoothingRadius * 0.28);
	Uniform<float> artificialViscosityMultiplier	= Uniform<float>("artificialViscosityMultiplier", 0.5);
	Uniform<float> artificialPressureMultiplier		= Uniform<float>("artificialPressureMultiplier", 2.0);

	//Elastic solids
	float stiffness = 0.0;

	//GPU Threading settings
	GLuint pThread = 3000000; //Max Number of particles (thread) (10 milion)

	//Solver settings
	int solver_substep = 4;
	int solver_iteration = 8;
	float overRelaxation = 1.0;

	//calulated
	GLuint pWkgSize = 1024; //Number of thread per workgroup
	GLuint bWkgSize = 256; //Number of thread per workgroup

	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed
	GLuint bThread = int(bb.x / (bWidth)) * int(bb.y / (bWidth)) * int(bb.z / (bWidth)); //Total number of bin (thread)
	GLuint blockSize = floor(log2f(bThread));
	GLuint blocks = (bThread + blockSize - 1) / blockSize;
	GLuint bWkgCount = (blocks + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed
};
