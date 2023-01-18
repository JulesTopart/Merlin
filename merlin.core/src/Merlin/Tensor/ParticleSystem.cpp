#include "glpch.h"
#include "GL.h"
#include "ParticleSystem.h"
#include "Merlin/Renderer/IndexBuffer.h"


namespace Merlin::Tensor {

	ParticleSystem::ParticleSystem(std::string name, GLsizeiptr maxCount) {
		_geometry = CreateShared<Primitive>();
		_particlesCount = maxCount;
		_model = glm::mat4(1.0f);
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
				buffer->Attach(shader);
			}

			shader->Use();
			// Set the uniform variables for the compute shader.
			GLuint uniLoc = glGetUniformLocation(shader->id(), "tstep");
			if (uniLoc != -1) shader->SetDouble("tstep", ts);

			// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
			shader->Dispatch(_particlesCount / 16, 1, 1);

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
		shader->Dispatch(_particlesCount / _threadPattern, 1, 1);

		// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	void ParticleSystem::Draw(Shared<Shader> shader, glm::mat4 view) {
		
		for (Shared<SSBO> buffer : _buffers) {
			buffer->Attach(shader);
		}
		_geometry->SetTransform(_model);
		_geometry->DrawInstanced(shader, view, _particlesCount);
	}


	void ParticleSystem::Translate(glm::vec3 v) {
		_model = glm::translate(_model, v);
	}

	void ParticleSystem::Rotate(glm::vec3 v) {
		_model = glm::rotate(_model, v.x, glm::vec3(1, 0, 0));
		_model = glm::rotate(_model, v.y, glm::vec3(0, 1, 0));
		_model = glm::rotate(_model, v.z, glm::vec3(0, 0, 1));
	}

	void ParticleSystem::Rotate(float angle, glm::vec3 v) {
		_model = glm::rotate(_model, angle, v);
	}


}