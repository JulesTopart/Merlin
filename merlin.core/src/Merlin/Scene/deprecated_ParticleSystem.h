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

namespace Merlin {

	enum class deprecated_ParticleSystemDisplayMode {
		MESH,
		SPRITE,
		POINT_SPRITE,
		POINT_SPRITE_SHADED
	};

	// Refer to Uniform block layout alignement: https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
    // Particle properties
	struct deprecated_DefaultParticle {
		alignas(16) glm::vec4 position;      // Position (x, y, z) and mass (w)
		alignas(16) glm::vec4 velocity;      // Velocity (x, y, z) and density (w)
	};

	class deprecated_GenericParticleSystem : public RenderableObject {
	public:
		deprecated_GenericParticleSystem();
		deprecated_GenericParticleSystem(const std::string& name, GLsizeiptr instances);

		void Draw(const Shader& shader) const; //Draw the mesh

		inline void SetInstancesCount(GLuint t) { m_instancesCount = t; }
		//Meshs
		inline void SetMesh(Shared<Mesh> geometry) { m_geometry = geometry; }
		inline Shared<Mesh> GetMesh() const { return m_geometry; }

		inline void SetDisplayMode(deprecated_ParticleSystemDisplayMode mode) { m_displayMode = mode; }
		inline deprecated_ParticleSystemDisplayMode GetDisplayMode() const { return m_displayMode; }

		inline void AddComputeShader(Shared<ComputeShader> cs) { m_shaders.push_back(cs); };

	protected : 
		GLsizeiptr m_instancesCount = 1;

		//Geometry
		Shared<Mesh> m_geometry;
		deprecated_ParticleSystemDisplayMode m_displayMode = deprecated_ParticleSystemDisplayMode::POINT_SPRITE;

		//Compute shaders
		std::vector<Shared<ComputeShader>> m_shaders; //Shader to compute the particle position

	};


	class deprecated_ParticleSystem : public deprecated_GenericParticleSystem {
	public:
		


		deprecated_ParticleSystem() : deprecated_GenericParticleSystem() {}
		deprecated_ParticleSystem(std::string name, GLsizeiptr maxCount) : deprecated_GenericParticleSystem(name, maxCount) {}
		~deprecated_ParticleSystem() {};

		//void Update(Timestep ts);//Execute all linked compute shader
		//void Execute(Shared<ComputeShader> step, bool autoBind = true); //Execute compute shader once

		inline static Shared<deprecated_ParticleSystem> Create(std::string name, GLsizeiptr instances) {
			return CreateShared<deprecated_ParticleSystem>(name, instances);
		};

		inline void AddStorageBuffer(Shared<GenericBufferObject> buffer) {
			m_buffers.push_back(buffer);
			if (m_shaders.size() == 0) Console::info("ParticleSystem") << "Shader list is empty. Load particle system shader before adding buffer to enable automatic shader attach or attach them manually" << Console::endl;
			for (Shared<ComputeShader> shader : m_shaders) {
				shader->Attach(*buffer);
			}
		}

	private:
		//Buffers & Compute Shaders
		std::vector<Shared<GenericBufferObject>> m_buffers; //Buffer to store the particle 
		
	};

	typedef Shared<deprecated_ParticleSystem> deprecated_ParticleSystem_Ptr;

}
