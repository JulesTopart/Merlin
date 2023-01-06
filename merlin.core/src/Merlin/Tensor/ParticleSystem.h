#pragma once

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
		alignas(16) glm::vec4 position;  //16  00
		alignas(16) glm::vec4 velocity;  //16  16
		float temperature;
		float conductivity;
		float capacity;
		float density;
		float pressure;
		float mass;
		//SPH : Density, Pressure
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

		//Primitives
		inline void SetPrimitive(Shared<Primitive> geometry) { _geometry = geometry; }
		inline Shared<Primitive> GetPrimitive() const { return _geometry; }

		inline const std::string name() const { return _name; }


	private:

		std::string _name;
		GLsizeiptr _particlesCount;

		//Geometry
		Shared<Primitive> _geometry;

		//Buffers & Compute Shaders
		std::vector<Shared<SSBO>> _buffers; //Buffer to store the particle
		std::vector<Shared<ComputeShader>> _shaders; //Shader to compute the particle position

	};
}
