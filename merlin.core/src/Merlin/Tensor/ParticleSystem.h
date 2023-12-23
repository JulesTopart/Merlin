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


	enum class ParticleSystemDisplayMode {
		MESH,
		SPRITE,
		POINT_SPRITE,
		POINT_SPRITE_SHADED
	};

	// Refer to Uniform block layout alignement: https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
    // Particle properties
	struct DefaultParticle {
		alignas(16) glm::vec4 position;      // Position (x, y, z) and mass (w)
		alignas(16) glm::vec4 velocity;      // Velocity (x, y, z) and density (w)
	};

	class GenericParticleSystem : public RenderableObject {
	public:
		GenericParticleSystem();
		GenericParticleSystem(const std::string& name, GLsizeiptr instances);

		void Draw(const Shader& shader) const; //Draw the mesh

		inline void SetInstancesCount(GLuint t) { m_instancesCount = t; }
		//Meshs
		inline void SetMesh(Shared<Mesh> geometry) { m_geometry = geometry; }
		inline Shared<Mesh> GetMesh() const { return m_geometry; }

		inline void SetDisplayMode(ParticleSystemDisplayMode mode) { m_displayMode = mode; }
		inline ParticleSystemDisplayMode GetDisplayMode() const { return m_displayMode; }

		inline void AddComputeShader(Shared<ComputeShader> cs) { m_shaders.push_back(cs); };

	protected : 
		GLsizeiptr m_instancesCount = 1;

		//Geometry
		Shared<Mesh> m_geometry;
		ParticleSystemDisplayMode m_displayMode = ParticleSystemDisplayMode::POINT_SPRITE;

		//Compute shaders
		std::vector<Shared<ComputeShader>> m_shaders; //Shader to compute the particle position

	};


	class ParticleSystem : public GenericParticleSystem {
	public:
		


		ParticleSystem() : GenericParticleSystem() {}
		ParticleSystem(std::string name, GLsizeiptr maxCount) : GenericParticleSystem(name, maxCount) {}
		~ParticleSystem() {};

		//void Update(Timestep ts);//Execute all linked compute shader
		//void Execute(Shared<ComputeShader> step, bool autoBind = true); //Execute compute shader once

		inline static Shared<ParticleSystem> Create(std::string name, GLsizeiptr instances) {
			return CreateShared<ParticleSystem>(name, instances);
		};

		inline void AddStorageBuffer(Shared<GenericBufferObject> buffer) {
			m_buffers.push_back(buffer);
			if (m_shaders.size() == 0) Console::info("ParticleSystem") << "Shader list is empty. Load particle system shader before adding buffer to enable automatic shader attach or attach them manually" << Console::endl;
			for (Shared<ComputeShader> shader : m_shaders) {
				shader->Attach(*buffer, m_buffers.size() - 1);
			}
		}

	private:
		//Buffers & Compute Shaders
		std::vector<Shared<GenericBufferObject>> m_buffers; //Buffer to store the particle
		
	};

	typedef Shared<ParticleSystem> ParticleSystem_Ptr;

}
