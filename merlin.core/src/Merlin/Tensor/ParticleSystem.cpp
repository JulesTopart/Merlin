#include "glpch.h"
#include "GL.h"
#include "ParticleSystem.h"
#include "Merlin/Memory/IndexBuffer.h"


namespace Merlin::Tensor {

	void GenericParticleSystem::Draw(const Shader& shader) const {
		m_geometry->DrawInstanced(m_activeInstancesCount);
	}

	template<class T>
	ParticleSystem<T>::ParticleSystem(std::string name, GLsizeiptr maxCount) : RenderableObject(name){
		m_activeInstancesCount = m_instancesCount = maxCount;
	}

	template<class T>
	ParticleSystem<T>::~ParticleSystem() {}


	template<class T>
	void ParticleSystem<T>::AddStorageBuffer(SSBO<T>& buffer) {
		m_buffers.push_back(buffer);
		if (m_shaders.size() == 0) Console::info("ParticleSystem") << "Shader list is empty. Load particle system shader before adding buffer to enable automatic shader attach or attach them manually" << Console::endl;
		for (Shared<ComputeShader> shader : m_shaders) {
			buffer->Bind();
			buffer->Attach(*shader, m_buffers.size()-1);
		}
	}

	template<class T>
	void ParticleSystem<T>::Update(Timestep ts) {

		for (Shared<ComputeShader> shader : m_shaders) {
			//for (Shared<SSBO> buffer : _buffers) {
				//buffer->Bind();
				//buffer->Attach(*shader); //Should be done before.
			//}

			shader->Use();
			// Set the uniform variables for the compute shader.
			//shader->SetFloat("dt", ts);

			// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
			shader->Dispatch((m_activeInstancesCount + m_wkGrpSize - 1) / m_wkGrpSize);

			// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}
	}

	template<class T>
	void ParticleSystem<T>::Execute(Shared<ComputeShader> shader, bool useAutoBind) {

		if (useAutoBind) {
			int i = 0;
			for (Shared<SSBO<T>> buffer : m_buffers) {
				buffer->Bind();
				buffer->Attach(*shader, i++);
			}
			shader->Use();
		}
		// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
		shader->Dispatch((m_activeInstancesCount + m_wkGrpSize - 1) / m_wkGrpSize); //Total number of workgroup needed, 1, 1);

		// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}




}