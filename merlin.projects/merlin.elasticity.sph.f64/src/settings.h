#pragma once
#include <Merlin.h>
#include <glm/glm.hpp>

struct Bin {
	GLuint count; //particle count in the bin
	GLuint sum;   //global sum
	GLuint csum;  //local sum
	GLuint index; //bin index
};

//Scale 
// length in mm
// density in g/cm3
// length in mm



struct Settings {
	const double smoothingRadius = 0.4;
	const double particleRadius = smoothingRadius/3.0;
	const double bWidth = smoothingRadius*2.0;

	//Boundary Volume dimensions 
	glm::dvec3 bb = glm::dvec3(280, 50, 8);

	// Physics Parameters
	Uniform<double> dt								= Uniform<double>("u_dt", 0.5e-4);
	Uniform<double> restDensity						= Uniform<double>("u_rho0", 1.43);//PLA g/cm3
	Uniform<double> particleMass					= Uniform<double>("u_mass", pow(smoothingRadius, 3.0) * restDensity.value()); //mg

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


	inline void setConstants(ShaderBase& shader) {
		shader.setConstDVec3("cst_domain", bb);
		shader.setConstDVec3("cst_halfdomain", bb*glm::dvec3(0.5));
		shader.setConstDVec3("cst_boundaryMin", bb*glm::dvec3(-0.5, -0.5, -0.5));
		shader.setConstDVec3("cst_boundaryMax", bb*glm::dvec3(0.5, 0.5, 0.5));

		shader.setConstDouble("cst_particleRadius", particleRadius);
		shader.setConstDouble("cst_smoothingRadius", smoothingRadius);
		shader.setConstDouble("cst_binSize", bWidth);
		shader.setConstUVec3("cst_binMax", glm::uvec3(bb / bWidth));
		shader.setConstUInt("cst_binCount", bThread);

		shader.define("PTHREAD", std::to_string(pWkgSize));
		shader.define("BTHREAD", std::to_string(bWkgSize));
	}

};
