#include "glpch.h"
#include "ParticleSystem.h"
#include "Merlin/Renderer/IndexBuffer.h"


namespace Merlin::Tensor {

	ParticleSystem::ParticleSystem(std::string name, GLsizeiptr maxCount) {
		_geometry = CreateShared<Primitive>();
		_particlesCount = 1;
	}

	ParticleSystem::~ParticleSystem() {}

	void ParticleSystem::AddComputeShader(Shared<ComputeShader> step) {
		_shaders.push_back(step);
	}

	void ParticleSystem::AddStorageBuffer(Shared<SSBO> buffer) {
		_buffers.push_back(buffer);
	}

	void ParticleSystem::Update(Timestep ts) {

		for (Shared<SSBO> buffer : _buffers) {
			buffer->Bind();
		}

		for (Shared<ComputeShader> shader : _shaders) {
			for (Shared<SSBO> buffer : _buffers) {
				buffer->Attach(shader);
			}

			shader->Use();
			// Set the uniform variables for the compute shader.
			GLuint uniLoc = glGetUniformLocation(shader->id(), "tstep");
			if (uniLoc != -1) shader->SetFloat("tstep", ts);

			// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
			shader->Dispatch(_particlesCount / 128, 1, 1);

			// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}
		
	}

	void ParticleSystem::Execute(Shared<ComputeShader> shader){

		for (Shared<SSBO> buffer : _buffers) {
			buffer->Bind();
			buffer->Attach(shader);
		}

		shader->Use();

		// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
		shader->Dispatch(_particlesCount / 128, 1, 1);

		// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		
		
	}

	void ParticleSystem::Draw(Shared<Shader> shader, glm::mat4 view) {
		
		for (Shared<SSBO> buffer : _buffers) {
			buffer->Attach(shader);
		}

		_geometry->Draw(shader, view);
	}


}