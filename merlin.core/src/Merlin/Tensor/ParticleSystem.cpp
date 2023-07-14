#include "glpch.h"
#include "GL.h"
#include "ParticleSystem.h"
#include "Merlin/Memory/IndexBuffer.h"


namespace Merlin::Tensor {

	ParticleSystem::ParticleSystem(std::string name, GLsizeiptr maxCount) : RenderableObject(name){
		_instancesCount = maxCount;
	}

	ParticleSystem::~ParticleSystem() {}

	void ParticleSystem::AddComputeShader(Shared<ComputeShader> step) {
		_shaders.push_back(step);
	}

	void ParticleSystem::AddStorageBuffer(Shared<SSBO> buffer) {
		_buffers.push_back(buffer);
	}

	void ParticleSystem::Update(Timestep ts) {

		for (Shared<ComputeShader> shader : _shaders) {
			for (Shared<SSBO> buffer : _buffers) {
				buffer->Bind();
				buffer->Attach(*shader);
			}

			shader->Use();
			// Set the uniform variables for the compute shader.
			//shader->SetFloat("dt", ts);

			// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
			shader->Dispatch(_instancesCount / _thread, 1, 1);

			// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}
		
	}

	void ParticleSystem::Execute(Shared<ComputeShader> shader, bool useAutoBind) {

		if (useAutoBind) {
			for (Shared<SSBO> buffer : _buffers) {
				buffer->Bind();
				buffer->Attach(*shader);
			}

			shader->Use();
		}
		// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
		shader->Dispatch(_instancesCount / _thread, 1, 1);

		// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}

	void ParticleSystem::Draw(const Shader& shader) const {
		
		for (Shared<SSBO> buffer : _buffers) {
			buffer->Attach(shader);
		}


		_geometry->DrawInstanced(_instancesCount);
	}


}