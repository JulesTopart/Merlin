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
	const float particleRadius = 0.25;
	const float smoothingRadius = 4.4 * particleRadius;
	const float bWidth = smoothingRadius;

	//Boundary Volume dimensions
	glm::vec2 bb = glm::vec2(300, 50);

	// Physics Parameters
	Uniform<float> timestep							= Uniform<float>("dt", 0.3);//ms
	Uniform<float> restDensity						= Uniform<float>("restDensity", 1.0);
	Uniform<float> particleMass						= Uniform<float>("particleMass", smoothingRadius * 0.28);
	Uniform<float> artificialViscosityMultiplier	= Uniform<float>("artificialViscosityMultiplier", 0.01);
	Uniform<float> artificialPressureMultiplier		= Uniform<float>("artificialPressureMultiplier", 2780000.0);
	Uniform<float> XSPH = Uniform<float>("XSPH", 0.034);

	//Elastic solids
	float stiffness = 0.0;

	//GPU Threading settings
	GLuint pThread = 1000000; //Max Number of particles (thread) (10 milion)

	//calulated
	GLuint pWkgSize = 1024; //Number of thread per workgroup
	GLuint bWkgSize = 128; //Number of thread per workgroup

	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed
	GLuint bThread = int(bb.x / (bWidth)) * int(bb.y / (bWidth)); //Total number of bin (thread)
	GLuint blockSize = floor(log2f(bThread));
	GLuint blocks = (bThread + blockSize - 1) / blockSize;
	GLuint bWkgCount = (blocks + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed
};
