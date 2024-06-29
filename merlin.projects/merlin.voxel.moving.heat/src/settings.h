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
	const float particleRadius = 0.05;

	// Physics Parameters
	Uniform<float> timestep							= Uniform<float>("dt", 0.0016);

	//Elastic solids
	float stiffness = 0.0;

	//GPU Threading settings
	GLuint pThread = 10000; //Max Number of particles (thread) (10 milion)

	//Solver settings
	int solver_substep = 3;
	int solver_iteration = 3;
	float overRelaxation = 1.0;

	//calulated
	GLuint pWkgSize = 32; //Number of thread per workgroup
	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

};
