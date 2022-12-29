#pragma once

#include "Merlin/Renderer/VertexArray.h"
#include "Merlin/Tensor/ShaderStorageBuffer.h"
#include "Merlin/Renderer/Shader.h"
#include "Merlin/Renderer/Texture.h"
#include "Merlin/Tensor/ComputeShader.h"
#include "Merlin/Core/Timestep.h"

#include "glm/gtc/random.hpp"

namespace Merlin::Tensor {

	using namespace Merlin::Renderer;

	struct DefaultParticle {
		glm::vec3 position;
		glm::vec3 velocity;
		float mass;
	};


	template <typename T>
	class ParticleSystem{
	public:
		ParticleSystem();
		~ParticleSystem();

		void Allocate(GLsizeiptr numParticles);


		void Update(Timestep ts);//Execute all linked compute shader
		void Execute(std::shared_ptr<ComputeShader> step); //Execute compute shader once
		void Draw(std::shared_ptr<Shader> shader, glm::mat4 view); //Draw the mesh

		void LoadVertex(std::vector<Vertex>& _vertices);
		void LoadVertex(std::vector<Vertex>& _vertices, std::vector<GLuint>& _indices);

		void AddComputeShader(std::shared_ptr<ComputeShader>);

		inline glm::mat4& GetModelMatrix() { return model; }

		void translate(glm::vec3);
		void rotate(glm::vec3);
		void rotate(float angle, glm::vec3 v);
		void SetDrawMode(GLuint mode);

		inline const std::string name() const { return _name; }

	private:

		std::string _name;

		//Geometry
		std::unique_ptr<VAO> vao;
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		GLuint drawMode = GL_TRIANGLES;

		std::vector<T> _particles;

		//Transformation
		glm::mat4 model;

		std::shared_ptr<SSBO> _particleInBuffer; //Buffer to store the particle
		std::shared_ptr<SSBO> _particleOutBuffer; //Buffer to store the particle
		std::vector<std::shared_ptr<ComputeShader>> _computeShaders; //Shader to compute the particle position

	};

	template class ParticleSystem<DefaultParticle>;
}
