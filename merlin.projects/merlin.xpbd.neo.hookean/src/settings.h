#pragma once
#include <Merlin.h>
#include <glm/glm.hpp>

struct Constraint {
	GLuint a;
	GLuint b;
	float restLength;
	float lambda;
};

struct Settings {
	const float particleRadius = 1;

	// Physics Parameters
	Uniform<float> timestep = Uniform<float>("dt", 0.016);
	Uniform<float> particleMass = Uniform<float>("particleMass", 1.0);

	//Elastic solids
	float stiffness = 0.0;

	//GPU Threading settings
	GLuint pThread = 100000; //Max Number of particles (thread) (100k)
	GLuint pWkgSize = 512; //Number of thread per workgroup
	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

	GLuint cThread = 300000; //Max Number of constraint (thread) (300k)
	GLuint cWkgSize = 256; //Number of thread per workgroup
	GLuint cWkgCount = (cThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

	//Solver settings
	int solver_substep = 3;
	int solver_iteration = 3;
	float overRelaxation = 1.0;

};
