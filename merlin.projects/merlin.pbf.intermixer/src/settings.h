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
	const float particleRadius = 0.48;
	const float smoothingRadius = 4 * particleRadius;
	const float bWidth = smoothingRadius;

	//Solver settings
	int solver_substep = 3;
	int solver_iteration = 3;
	float overRelaxation = 1.0;

	//Boundary Volume dimensions
	glm::vec3 bb = glm::vec3(240, 130, 150);

	// Physics Parameters
	float timestep									= 0.001;
	Uniform<float> dt								= Uniform<float>("u_dt", timestep / solver_substep);
	Uniform<float> restDensity						= Uniform<float>("u_rho0", 1.0);
	Uniform<float> particleMass						= Uniform<float>("u_mass", (4.0/3.0)*glm::pi<float>() * particleRadius * particleRadius * particleRadius);

	Uniform<float> viscosity						= Uniform<float>("u_viscosity", 0.5);
	Uniform<float> artificialViscosityMultiplier	= Uniform<float>("u_artificialViscosityMultiplier", 45 * 0.01);
	Uniform<float> artificialPressureMultiplier		= Uniform<float>("u_artificialPressureMultiplier",  4 * 0.001);

	float emitterDelay = 0.120;//ms


	//calculated (don't change value here)
	Uniform<GLuint> numParticles					= Uniform<GLuint>("u_numParticles", 0);
	Uniform<GLuint> numEmitter						= Uniform<GLuint>("u_numEmitter", 0);

	Uniform<glm::mat4> emitter_transform			= Uniform<glm::mat4>("u_emitterTransform", glm::mat4(1));


	//Elastic solids
	float stiffness = 0.0;

	//GPU Threading settings
	GLuint max_pThread = 2000000; //Max Number of particles (thread) (10 milion)
	GLuint pThread = 1;
	GLuint pWkgSize = 1024; //Number of thread per workgroup
	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

	GLuint bWkgSize = 512; //Number of thread per workgroup
	GLuint bThread = int(bb.x / (bWidth)) * int(bb.y / (bWidth)) * int(bb.z / (bWidth)); //Total number of bin (thread)
	GLuint blockSize = floor(log2f(bThread));
	GLuint blocks = (bThread + blockSize - 1) / blockSize;
	GLuint bWkgCount = (blocks + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed

	GLuint cThread = max_pThread * 32; //Max Number of constraint (thread) (32 * 1M)



	inline void setTimestep(float t) {
		timestep = t;
		dt = timestep / solver_substep;
	}


	inline void setConstants(ShaderBase& shader) {
		shader.setConstVec3("cst_domain", bb);
		shader.setConstVec3("cst_halfdomain", bb*glm::vec3(0.5));
		shader.setConstVec3("cst_boundaryMin", bb*glm::vec3(-0.5, -0.5, 0));
		shader.setConstVec3("cst_boundaryMax", bb*glm::vec3(0.5, 0.5, 1));

		shader.setConstFloat("cst_particleRadius", particleRadius);
		shader.setConstFloat("cst_smoothingRadius", smoothingRadius);
		shader.setConstFloat("cst_binSize", bWidth);
		shader.setConstUVec3("cst_binMax", glm::uvec3(bb / bWidth));
		shader.setConstUInt("cst_binCount", bThread);

		shader.define("PTHREAD", std::to_string(pWkgSize));
		shader.define("BTHREAD", std::to_string(bWkgSize));
	}

};
