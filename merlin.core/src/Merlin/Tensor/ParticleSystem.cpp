#include "glpch.h"
#include "GL.h"
#include "ParticleSystem.h"
#include "Merlin/Memory/IndexBuffer.h"


namespace Merlin::Tensor {

	ParticleSystem::ParticleSystem(std::string name, GLsizeiptr maxCount) : RenderableObject(name){
		_activeInstancesCount = _instancesCount = maxCount;
	}

	ParticleSystem::~ParticleSystem() {}

	void ParticleSystem::AddComputeShader(Shared<ComputeShader> step) {
		_shaders.push_back(step);
	}

	void ParticleSystem::AddStorageBuffer(Shared<SSBO> buffer) {
		_buffers.push_back(buffer);
		if (_shaders.size() == 0) Console::info("ParticleSystem") << "Shader list is empty. Load particle system shader before adding buffer to enable automatic shader attach or attach them manually" << Console::endl;
		for (Shared<ComputeShader> shader : _shaders) {
			buffer->Bind();
			buffer->Attach(*shader, _buffers.size()-1);
		}
	}

	void ParticleSystem::Update(Timestep ts) {

		for (Shared<ComputeShader> shader : _shaders) {
			//for (Shared<SSBO> buffer : _buffers) {
				//buffer->Bind();
				//buffer->Attach(*shader); //Should be done before.
			//}

			shader->Use();
			// Set the uniform variables for the compute shader.
			//shader->SetFloat("dt", ts);

			// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
			shader->Dispatch((_activeInstancesCount + _thread - 1) / _thread);

			// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}
	}

	void ParticleSystem::Execute(Shared<ComputeShader> shader, bool useAutoBind) {

		if (useAutoBind) {
			int i = 0;
			for (Shared<SSBO> buffer : _buffers) {
				buffer->Bind();
				buffer->Attach(*shader, i++);
			}
			shader->Use();
		}
		// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
		shader->Dispatch((_activeInstancesCount + _thread - 1) / _thread); //Total number of workgroup needed, 1, 1);

		// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}

	void ParticleSystem::Draw(const Shader& shader) const {
		
		int i = 0;
		for (Shared<SSBO> buffer : _buffers) {
			buffer->Attach(shader, i++);
		}


		_geometry->DrawInstanced(_activeInstancesCount);
	}


}