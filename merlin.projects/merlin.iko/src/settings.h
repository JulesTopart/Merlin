#pragma once
#include <Merlin.h>
#include <glm/glm.hpp>

struct Bin {
	GLuint count; //particle count in the bin
	GLuint sum;   //global sum
	GLuint csum;  //local sum
	GLuint index; //bin index
};

struct Settings {
	const float particleRadius = 0.3;
	const float smoothingRadius = 4 * particleRadius;
	const float bWidth = smoothingRadius;
	const float volumeWidth = smoothingRadius*0.75;

	//Boundary Volume dimensions
	glm::vec3 bb = glm::vec3(330, 80, 180);


	// Physics Parameters
	Uniform<float> timestep							= Uniform<float>("dt", 0.0016);
	Uniform<float> restDensity						= Uniform<float>("restDensity", 1.0);
	Uniform<float> particleMass						= Uniform<float>("particleMass", 3.0*smoothingRadius * 0.28);
	Uniform<float> artificialViscosityMultiplier	= Uniform<float>("artificialViscosityMultiplier", 8);
	Uniform<float> artificialPressureMultiplier		= Uniform<float>("artificialPressureMultiplier", 4.0);

	//Elastic solids
	float stiffness = 0.0;

	//GPU Threading settings
	GLuint pThread = 3000000; //Max Number of particles (thread) (10 milion)

	//Solver settings
	int solver_substep = 3;
	int solver_iteration = 3;
	float overRelaxation = 1.0;

	//calulated
	GLuint pWkgSize = 1024; //Number of thread per workgroup
	GLuint bWkgSize = 512; //Number of thread per workgroup
	

	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed
	GLuint bThread = int(bb.x / (bWidth)) * int(bb.y / (bWidth)) * int(bb.z / (bWidth)); //Total number of bin (thread)
	
	glm::ivec3 iWkgSize = glm::ivec3(6); //Number of thread per workgroup
	glm::ivec3 volume_size = glm::ivec3(int(bb.x / (volumeWidth)), int(bb.y / (volumeWidth)), int(bb.z / (volumeWidth)));
	glm::ivec3 iWkgCount = (volume_size + iWkgSize - glm::ivec3(1)) / iWkgSize; //Total number of workgroup needed


	GLuint blockSize = floor(log2f(bThread));
	GLuint blocks = (bThread + blockSize - 1) / blockSize;
	GLuint bWkgCount = (blocks + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed
};
