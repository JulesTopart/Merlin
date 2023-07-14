#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Memory/VertexArray.h"
#include "Merlin/Memory/ShaderStorageBuffer.h"
#include "Merlin/Graphics/Shader.h"
#include "Merlin/Memory/Texture.h"
#include "Merlin/Tensor/ComputeShader.h"
#include "Merlin/Core/Timestep.h"
#include "Merlin/Graphics/Mesh.h"

#include "glm/gtc/random.hpp"

namespace Merlin::Tensor {

	using namespace Merlin::Graphics;


	// Refer to Uniform block layout alignement: https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
    // Particle properties
	struct DefaultParticle {
		alignas(16) glm::vec4 position;      // Position (x, y, z) and mass (w)
		alignas(16) glm::vec4 velocity;      // Velocity (x, y, z) and density (w)
	};

	class ParticleSystem : public RenderableObject{
	public:
		ParticleSystem(std::string name, GLsizeiptr maxCount);
		~ParticleSystem();

		void Update(Timestep ts);//Execute all linked compute shader
		void Execute(Shared<ComputeShader> step, bool autoBind = true); //Execute compute shader once

		void Draw(const Shader& shader) const; //Draw the mesh

		void AddComputeShader(Shared<ComputeShader>);
		void AddStorageBuffer(Shared<SSBO>);

		inline void SetThread(GLuint t) { _thread = t; }
		//Meshs
		inline void SetMesh(Shared<Mesh> geometry) { _geometry = geometry; }
		inline Shared<Mesh> GetMesh() const { return _geometry; }

	private:
		GLsizeiptr _instancesCount;
		GLuint _thread = 64;
			
		//Geometry
		Shared<Mesh> _geometry;

		//Buffers & Compute Shaders
		std::vector<Shared<SSBO>> _buffers; //Buffer to store the particle
		std::vector<Shared<ComputeShader>> _shaders; //Shader to compute the particle position

	};
}
