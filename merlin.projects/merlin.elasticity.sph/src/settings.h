#pragma once
#include <Merlin.h>
#include <glm/glm.hpp>

struct Bin {
	GLuint count; //particle count in the bin
	GLuint sum;   //global sum
	GLuint csum;  //local sum
	GLuint index; //bin index
};

struct sym_tensor3x3 {
	float xx;
	float yy;
	float zz;
	float xy;
	float yz;
	float zx;
};

struct Settings {
	const float particleRadius = 0.4;
	const float smoothingRadius = 4 * particleRadius;
	const float bWidth = smoothingRadius*1.0;

	//Boundary Volume dimensions 
	glm::vec3 bb = glm::vec3(280, 50, 8);

	// Physics Parameters
	Uniform<float> dt								= Uniform<float>("u_dt", 0.5e-4);
	Uniform<float> restDensity						= Uniform<float>("u_rho0", 1.43);//PLA
	Uniform<float> particleMass = Uniform<float>("u_mass", particleRadius * particleRadius * particleRadius * restDensity.value());

	Uniform<float> viscosity						= Uniform<float>("u_viscosity", 0.0);
	Uniform<float> artificialViscosityMultiplier	= Uniform<float>("u_artificialViscosityMultiplier", 0 * 0.01);
	Uniform<float> artificialPressureMultiplier		= Uniform<float>("u_artificialPressureMultiplier",  0 * 0.001);

	//calculated (don't change value here)
	Uniform<GLuint> numParticles					= Uniform<GLuint>("u_numParticles", 0);

	//Elastic solids
	float stiffness = 0.0;

	//GPU Threading settings
	GLuint pThread = 1;
	GLuint pWkgSize = 512; //Number of thread per workgroup
	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

	GLuint bWkgSize = 64; //Number of thread per workgroup
	GLuint bThread = int(bb.x / (bWidth)) * int(bb.y / (bWidth)) * int(bb.z / (bWidth)); //Total number of bin (thread)
	GLuint blockSize = floor(log2f(bThread));
	GLuint blocks = (bThread + blockSize - 1) / blockSize;
	GLuint bWkgCount = (blocks + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed


	inline void setConstants(ShaderBase& shader) {
		shader.setConstVec3("cst_domain", bb);
		shader.setConstVec3("cst_halfdomain", bb*glm::vec3(0.5));
		shader.setConstVec3("cst_boundaryMin", bb*glm::vec3(-0.5, -0.5, -0.5));
		shader.setConstVec3("cst_boundaryMax", bb*glm::vec3(0.5, 0.5, 0.5));

		shader.setConstFloat("cst_particleRadius", particleRadius);
		shader.setConstFloat("cst_smoothingRadius", smoothingRadius);
		shader.setConstFloat("cst_binSize", bWidth);
		shader.setConstUVec3("cst_binMax", glm::uvec3(bb / bWidth));
		shader.setConstUInt("cst_binCount", bThread);

		shader.define("PTHREAD", std::to_string(pWkgSize));
		shader.define("BTHREAD", std::to_string(bWkgSize));
	}

};
