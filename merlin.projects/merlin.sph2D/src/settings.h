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

/*
struct FluidParticle {
	alignas(16) glm::vec4 initial_position;	// initial position    x0
	alignas(16) glm::vec4 pposition;			// predicted position  x~
	alignas(16) glm::vec4 position;			// current position    x
	alignas(16) glm::vec4 new_position;		// predicted position  x+1
	alignas(16) glm::vec4 velocity;			// velocity			   u
	alignas(16) glm::vec4 new_velocity;		// velocity			   u*
	alignas(16) glm::vec4 acceleration;		// acceleration		   a
	GLfloat mass;				// mass				   m   (or pseudo mass for boundary particles)
	GLfloat density;			// density			   rho
	GLfloat pressure;			// pressure			   P
	GLfloat temperature;		// temperature		   T
	GLfloat new_temperature;	// new temperature	   T+1
	GLfloat lambda;			// lagrange multiplier lambda
	GLfloat dummyA;			//available
	GLfloat dummyB;			//available
	GLfloat dummyC;			//available
	GLuint phase;				// phase (liquid, solid...)
	GLuint newIndex;			// sorted index
	GLuint binIndex;			// bin index
};*/

struct Settings {
 
	//Boundary Volume dimensions
	glm::vec2 bb = glm::vec2(150, 80);

	// Physics Parameters
	float timeStep = 0.0001;//s
	float particleRadius = 0.25; // mm
	float particleMass = 1.0 * particleRadius;//g Mass

	float smoothingRadius = 2.4 * particleRadius; // SPH Kernel radius mm
	float restDensity = 1.0; // Rest density g/mm3

	float artificialViscosityMultiplier = 0.0;
	float artificialPressureMultiplier = 0.0;

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
	float bWidth = smoothingRadius; //Width of a single bin in mm
	GLuint bThread = int(bb.x / (bWidth)) * int(bb.y / (bWidth)); //Total number of bin (thread)
	GLuint blockSize = floor(log2f(bThread));
	GLuint blocks = (bThread + blockSize - 1) / blockSize;
	GLuint bWkgCount = (blocks + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed


};
