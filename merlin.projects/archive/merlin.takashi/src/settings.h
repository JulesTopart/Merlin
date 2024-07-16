#pragma once
#include <Merlin.h>
#include <glm/glm.hpp>

struct Bin {
	GLuint count; //particle count in the bin
	GLuint sum;   //global sum
	GLuint csum;  //local sum
	GLuint index; //bin index
};

struct Constraint {
	GLuint a[32];
	GLuint b[32];
	float restLength[32];
	GLuint count;
};

struct Settings {
	const float particleRadius = 0.4;
	const float smoothingRadius = 4 * particleRadius;
	const float bWidth = smoothingRadius;


	//Boundary Volume dimensions
	glm::vec3 bb = glm::vec3(200, 200, 100);

	// Physics Parameters
	Uniform<float> timestep							= Uniform<float>("dt", 0.0016);
	Uniform<float> restDensity						= Uniform<float>("restDensity", 1.0);
	Uniform<float> particleMass						= Uniform<float>("particleMass", 0.160);

	Uniform<float> artificialViscosityMultiplier	= Uniform<float>("artificialViscosityMultiplier", 0.1);
	Uniform<float> artificialPressureMultiplier		= Uniform<float>("artificialPressureMultiplier", 0.1);

	Uniform<float> extensionViscosity = Uniform<float>("extensionViscosity", 1.005);


	//Elastic solids
	float stiffness = 0.0;

	//GPU Threading settings
	GLuint max_pThread = 1000000; //Max Number of particles (thread) (10 milion)
	GLuint pThread = 1;
	GLuint pWkgSize = 1024; //Number of thread per workgroup
	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

	GLuint bWkgSize = 512; //Number of thread per workgroup
	GLuint bThread = int(bb.x / (bWidth)) * int(bb.y / (bWidth)) * int(bb.z / (bWidth)); //Total number of bin (thread)
	GLuint blockSize = floor(log2f(bThread));
	GLuint blocks = (bThread + blockSize - 1) / blockSize;
	GLuint bWkgCount = (blocks + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed

	GLuint cThread = max_pThread * 32; //Max Number of constraint (thread) (32 * 1M)


	//Solver settings
	int solver_substep = 3;
	int solver_iteration = 3;
	float overRelaxation = 1.0;


};
