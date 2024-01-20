#pragma once
#include <Merlin.h>
#include <glm/glm.hpp>


struct Bin {
	GLuint count; //particle count in the bin
	GLuint sum;   //global sum
	GLuint csum;  //local sum
	GLuint index; //bin index
};

struct FluidParticle {
	alignas(16) glm::vec4 position; // current position    x
	alignas(16) glm::vec4 velocity;	// velocity			   u
	GLfloat	invmass;				//inverse mass
	GLuint phase;					// phase (liquid, solid...)
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

	//Build Volume dimensions
	glm::vec3 bb = glm::vec3(30, 30, 100);
	float bx = bb.x;//mm 120
	float by = bb.y;//mm
	float bz = bb.z;//mm 40

	GLuint bRes = 128; //Bed width is divided bRes times (old 42)

	GLuint maxNNS = 32;

	//ex : volume = (100,40,40) & nozzle = 0.8 -> 312.500 particles; nozzle = 0.4 -> 2.500.000 particles)
	//float pDiameter = 1; //mm
	//GLuint pThread = int(bx / (pDiameter)) * int(by / (pDiameter)) * int(bz / (pDiameter)); //Max Number of particles (thread)
	GLuint pThread = 1000000; //Max Number of particles (thread) (10 milion)
	GLuint pWkgSize = 512; //Number of thread per workgroup
	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed


	float bWidth = std::max(bx, std::max(by, bz)) / float(bRes); //Width of a single bin in mm
	GLuint bThread = int(bx / (bWidth)) * int(by / (bWidth)) * int(bz / (bWidth)); //Total number of bin (thread)
	GLuint blockSize = floor(log2f(bThread));
	GLuint blocks = (bThread + blockSize - 1) / blockSize;

	GLuint bWkgSize = 512; //Number of thread per workgroup
	GLuint bWkgCount = (blocks + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed

	// --- SPH ---
	// SPH Parameters
	float particleRadius = 0.2; // mm
	float H = 3 * particleRadius; // Kernel radius mm
	float REST_DENSITY = 1.0; // g/mm3 Metled plastic
	float particleMass = 1.0;//g Mass
	float timeStep = 0.016;//g Mass

	int solver_substep = 30;
	int solver_iteration = 1;
};
