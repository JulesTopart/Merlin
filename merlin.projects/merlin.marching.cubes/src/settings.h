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
	glm::ivec3 volume_size = glm::ivec3(16, 16, 16);

	//calulated
	glm::ivec3 pWkgSize = glm::ivec3(8, 8, 8); //Number of thread per workgroup
	glm::ivec3 pWkgCount = (volume_size + pWkgSize - glm::ivec3(1)) / pWkgSize; //Total number of workgroup needed

};
