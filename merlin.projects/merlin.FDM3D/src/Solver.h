#pragma once

#include "Merlin/Memory/ShaderStorageBuffer.h"
#include "Merlin/Memory/Texture.h"
#include "Merlin/Tensor/ComputeShader.h"
#include "Merlin/Core/Timestep.h"
#include "Merlin/Util/Primitives.h"

using namespace Merlin;
using namespace Merlin::Tensor;

class Solver{
public:
	Solver(GLsizeiptr instances, GLuint thread = 16);
	~Solver(){};

	void Execute(); //Execute all compute shader
	void Execute(Shared<ComputeShader> shader); //Execute compute shader i once
	void Execute(Shared<ComputeShader> shader, std::vector<Shared<SSBO>> buffers); //Execute compute shader i once

	void AddComputeShader(Shared<ComputeShader>);
	void AddStorageBuffer(Shared<SSBO>);

	inline void SetThread(GLuint t) { _threadPattern = t; }

private:

	GLsizeiptr _instances;
	GLuint _threadPattern = 16;
	//Buffers & Compute Shaders
	std::vector<Shared<SSBO>> _buffers; //Buffer to store the particle
	std::vector<Shared<ComputeShader>> _shaders; //Shader to compute the particle position
};

