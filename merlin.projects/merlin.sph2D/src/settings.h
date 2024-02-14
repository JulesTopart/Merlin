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
	alignas(8) glm::vec2 position;
	alignas(8) glm::vec2 pposition;
	alignas(8) glm::vec2 velocity;
	alignas(8) glm::vec2 pvelocity;
	float mass;
	float density;
	float lambda;
	GLuint phase;
	GLuint binIndex;
	GLuint id;
	float padding[2];
};

struct Settings {
 
	//Boundary Volume dimensions
	glm::vec2 bb = glm::vec2(150, 80);

	// Physics Parameters
	Uniform<float> timestep							= Uniform<float>("dt", 0.0001);
	Uniform<float> particleRadius					= Uniform<float>("particleRadius", 1);
	Uniform<float> particleMass						= Uniform<float>("particleMass", 1.0 * particleRadius.value());
	Uniform<float> smoothingRadius					= Uniform<float>("smoothingRadius", 2.4 * particleRadius.value());
	Uniform<float> restDensity						= Uniform<float>("restDensity", 1.0);
	Uniform<float> artificialViscosityMultiplier	= Uniform<float>("artificialViscosityMultiplier", 0.0);
	Uniform<float> artificialPressureMultiplier		= Uniform<float>("artificialPressureMultiplier", 0.0);
	Uniform<float> bWidth							= Uniform<float>("bWidth", smoothingRadius.value());


	//Elastic solids
	float stiffness = 0.0;

	//GPU Threading settings
	GLuint pThread = 1000000; //Max Number of particles (thread) (10 milion)

	//Solver settings
	int solver_substep = 15;
	int solver_iteration = 1;
	float overRelaxation = 1.0;

	//calulated
	GLuint pWkgSize = 512; //Number of thread per workgroup
	GLuint bWkgSize = 512; //Number of thread per workgroup

	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed
	GLuint bThread = int(bb.x / (bWidth.value())) * int(bb.y / (bWidth.value())); //Total number of bin (thread)
	GLuint blockSize = floor(log2f(bThread));
	GLuint blocks = (bThread + blockSize - 1) / blockSize;
	GLuint bWkgCount = (blocks + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed
};
