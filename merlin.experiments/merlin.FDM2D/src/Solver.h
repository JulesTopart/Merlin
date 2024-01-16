#pragma once

#include "Merlin/Memory/ShaderStorageBuffer.h"
#include "Merlin/Renderer/Texture.h"
#include "Merlin/Tensor/ComputeShader.h"
#include "Merlin/Core/Timestep.h"
#include "Merlin/Renderer/Primitive.h"

using namespace Merlin;
using namespace Merlin::Tensor;

class Solver{
public:
	Solver(GLuint instances, GLuint thread = 16);
	~Solver(){};

	void Execute(); //Execute all compute shader
	void Execute(int i); //Execute compute shader i once

	void AddComputeShader(Shared<ComputeShader>);
	void AddStorageBuffer(Shared<SSBO>);

	inline void SetThread(GLuint t) { _threadPattern = t; }

private:

	GLuint _instances, _threadPattern = 16;
	//Buffers & Compute Shaders
	std::vector<Shared<SSBO>> _buffers; //Buffer to store the particle
	std::vector<Shared<ComputeShader>> _shaders; //Shader to compute the particle position
};

