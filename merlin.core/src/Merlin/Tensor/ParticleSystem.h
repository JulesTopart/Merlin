#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Renderer/VertexArray.h"
#include "Merlin/Tensor/ShaderStorageBuffer.h"
#include "Merlin/Renderer/Shader.h"
#include "Merlin/Renderer/Texture.h"
#include "Merlin/Tensor/ComputeShader.h"
#include "Merlin/Core/Timestep.h"
#include "Merlin/Renderer/Primitive.h"

#include "glm/gtc/random.hpp"

namespace Merlin::Tensor {

	using namespace Merlin::Renderer;


	//Refer to Uniform block layout alignement: https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
	struct DefaultParticle {
		alignas(64) glm::dvec3 position;
		alignas(64) glm::dvec3 velocity;
		double temperature;
		double conductivity;
		double capacity;
		double density;
		double pressure;
		double mass;
	};

	

	class ParticleSystem{
	public:
		ParticleSystem(std::string name, GLsizeiptr maxCount);
		~ParticleSystem();

		void Update(Timestep ts);//Execute all linked compute shader
		void Execute(Shared<ComputeShader> step); //Execute compute shader once
		void Draw(Shared<Shader> shader, glm::mat4 view); //Draw the mesh

		

		void AddComputeShader(Shared<ComputeShader>);
		void AddStorageBuffer(Shared<SSBO>);

		inline void SetThread(GLuint t) { _threadPattern = t; }

		//Primitives
		inline void SetPrimitive(Shared<Primitive> geometry) { _geometry = geometry; }
		inline Shared<Primitive> GetPrimitive() const { return _geometry; }

		inline const std::string name() const { return _name; }

		//Transformation
		void Translate(glm::vec3);
		void Rotate(glm::vec3);
		void Rotate(float angle, glm::vec3 v);


	private:

		std::string _name;
		GLsizeiptr _particlesCount;
		GLuint _threadPattern = 16;

		//Geometry
		Shared<Primitive> _geometry;
		//Transformation
		glm::mat4 _model;

		//Buffers & Compute Shaders
		std::vector<Shared<SSBO>> _buffers; //Buffer to store the particle
		std::vector<Shared<ComputeShader>> _shaders; //Shader to compute the particle position

	};
}
