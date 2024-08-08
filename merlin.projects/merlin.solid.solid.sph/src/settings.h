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
	const float smoothingRadius = 1.2;
	const float particleRadius = smoothingRadius/3.0;
	const float bWidth = smoothingRadius*2.0;

	//Boundary Volume dimensions 
	glm::vec3 bb = glm::vec3(100, 100, 60);

	// Physics Parameters
	Uniform<float> dt								= Uniform<float>("u_dt", 6e-3);
	Uniform<float> restDensity						= Uniform<float>("u_rho0", 1.43);//PLA g/cm3
	Uniform<float> particleMass						= Uniform<float>("u_mass", pow(smoothingRadius, 3.0) * restDensity.value()); //mg

	//calculated (don't change value here)
	Uniform<GLuint> numParticles					= Uniform<GLuint>("u_numParticles", 0);

	//GPU Threading settings
	GLuint pThread = 1;
	GLuint pWkgSize = 512; //Number of thread per workgroup
	GLuint pWkgCount = (pThread + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed

	GLuint bWkgSize = 64; //Number of thread per workgroup
	GLuint bThread = int(bb.x / (bWidth)) * int(bb.y / (bWidth)) * int(bb.z / (bWidth)); //Total number of bin (thread)
	GLuint blockSize = floor(log2f(bThread));
	GLuint blocks = (bThread + blockSize - 1) / blockSize;
	GLuint bWkgCount = (blocks + bWkgSize - 1) / bWkgSize; //Total number of workgroup needed

	glm::ivec3 texWkgSize = glm::ivec3(8, 8, 1); //Number of thread per workgroup
	glm::ivec3 tex_size = glm::ivec3(int(bb.x / (bWidth * 0.1)), int(bb.y / (bWidth * 0.1)), int(bb.z / (bWidth * 0.1)));


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
