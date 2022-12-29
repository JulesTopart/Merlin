#include "glpch.h"
#include "ParticleSystem.h"
#include "Merlin/Renderer/IndexBuffer.h"


namespace Merlin::Tensor {

	template <typename T>
	ParticleSystem<T>::ParticleSystem() {
		_particleInBuffer = std::make_shared<SSBO>(1);
		_particleOutBuffer = std::make_shared<SSBO>(2);
		vao = std::make_unique<VAO>();
		model = glm::mat4(1.0f); //Set to identity
	}

	template <typename T>
	ParticleSystem<T>::~ParticleSystem() {}

	template<typename T>
	void ParticleSystem<T>::Allocate(GLsizeiptr numParticles) {
		// Create the buffer object with the size of the particle data.

		std::vector<T> particles;
		for (size_t i(0); i < numParticles; i++) {
			particles.emplace_back();
		}

		Console::info("ParticleSystem") << "Allocating space for " << particles.size() << " particles" << Console::endl;

		_particles = particles;
		_particleInBuffer->Bind();
		_particleInBuffer->Allocate(_particles.size() * sizeof(T), particles.data());

		_particleOutBuffer->Bind();
		_particleOutBuffer->Allocate(_particles.size() * sizeof(T), particles.data());

		_particleInBuffer->Unbind();
		_particleOutBuffer->Unbind();
	}

	template <typename T>
	void ParticleSystem<T>::LoadVertex(std::vector<Vertex>& _vertices) {

		vertices = _vertices;

		Console::info("Mesh") << "Loaded " << vertices.size() << " verticles." << Console::endl;

		vao->Bind();
		VBO vbo(_vertices);

		vao->AddBuffer(vbo, Vertex::GetLayout());
		vao->Unbind();
	}

	template <typename T>
	void ParticleSystem<T>::LoadVertex(std::vector<Vertex>& _vertices, std::vector<GLuint>& _indices) {

		vertices = std::move(_vertices);
		_vertices.clear();
		indices = std::move(_indices);
		_indices.clear();

		Console::info("Mesh") << "Loaded " << indices.size() << " facets." << Console::endl;

		vao->Bind();
		VBO vbo(vertices);
		EBO ebo(indices);

		VertexBufferLayout layout = Vertex::GetLayout();
		vao->AddBuffer(vbo, layout);

		vao->Unbind();
		ebo.Unbind();
	}

	template <typename T>
	void ParticleSystem<T>::AddComputeShader(std::shared_ptr<ComputeShader> step) {
		_computeShaders.push_back(step);
	}

	template <typename T>
	void ParticleSystem<T>::Update(Timestep ts) {

		for (std::shared_ptr<ComputeShader> step : _computeShaders) {
			_particleInBuffer->Attach(step, "ParticleInBuffer");
			_particleOutBuffer->Attach(step, "ParticleOutBuffer");

			step->Use();
			// Set the uniform variables for the compute shader.
			step->SetFloat("tstep", ts);

			// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
			step->Dispatch(_particles.size() / 64, 1, 1);

			// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

			//Copy the result in output buffer in the input buffer for next step
			_particleInBuffer->Bind(GL_COPY_WRITE_BUFFER);
			_particleOutBuffer->Bind(GL_COPY_READ_BUFFER);

			//Copy the ouput buffer into the input buffer
			glCopyBufferSubData(
				GL_COPY_READ_BUFFER,
				GL_COPY_WRITE_BUFFER,
				0,
				0,
				_particles.size() * sizeof(T)
			);
		}
		
	}

	template <typename T>
	void ParticleSystem<T>::Execute(std::shared_ptr<ComputeShader> step) {
		// Bind the buffer object.
		_particleInBuffer->Attach(step, "ParticleInBuffer");
		_particleOutBuffer->Attach(step, "ParticleOutBuffer");
		step->Use();
		// Dispatch the compute shader, specifying the number of work groups to execute and the number of threads per work group.
		step->Dispatch(_particles.size() / 64, 1, 1);

		// Use glMemoryBarrier to ensure that the compute shader has finished writing to the buffer object before the CPU reads from it.
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	template <typename T>
	void ParticleSystem<T>::Draw(std::shared_ptr<Shader> shader, glm::mat4 view) {
		
		shader->Use();
		shader->SetMatrix4f("view", view); //Sync camera with GPU
		shader->SetMatrix4f("model", model); //Sync camera with GPU
		_particleInBuffer->Attach(shader, "ParticleInBuffer");
		_particleOutBuffer->Attach(shader, "ParticleOutBuffer");
		vao->Bind();
		if (indices.size() > 0) glDrawElementsInstanced(drawMode, indices.size(), GL_UNSIGNED_INT, nullptr, _particles.size()); //Draw elements using EBO
		else glDrawArraysInstanced(drawMode, 0, vertices.size(), _particles.size()); //Draw
		vao->Unbind();
	}

	template <typename T>
	void ParticleSystem<T>::SetDrawMode(GLuint mode) {
		drawMode = mode;
	}

	template <typename T>
	void ParticleSystem<T>::translate(glm::vec3 v) {
		model = glm::translate(model, v);
	}

	template <typename T>
	void ParticleSystem<T>::rotate(glm::vec3 v) {
		model = glm::rotate(model, v.x, glm::vec3(1, 0, 0));
		model = glm::rotate(model, v.y, glm::vec3(0, 1, 0));
		model = glm::rotate(model, v.z, glm::vec3(0, 0, 1));
	}

	template <typename T>
	void ParticleSystem<T>::rotate(float angle, glm::vec3 v) {
		model = glm::rotate(model, angle, v);
	}


}