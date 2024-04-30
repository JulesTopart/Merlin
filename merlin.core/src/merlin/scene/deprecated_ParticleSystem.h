#pragma once
#include "merlin/core/core.h"

#include "merlin/memory/vertexArray.h"
#include "merlin/memory/shaderStorageBuffer.h"
#include "merlin/shaders/shader.h"
#include "merlin/memory/texture.h"
#include "merlin/shaders/computeShader.h"
#include "merlin/core/timestep.h"
#include "merlin/graphics/mesh.h"

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

		void draw(const Shader& shader) const; //draw the mesh

		inline void setInstancesCount(GLuint t) { m_instancesCount = t; }
		//Meshs
		inline void setMesh(Shared<Mesh> geometry) { m_geometry = geometry; }
		inline Shared<Mesh> getMesh() const { return m_geometry; }

		inline void setDisplayMode(deprecated_ParticleSystemDisplayMode mode) { m_displayMode = mode; }
		inline deprecated_ParticleSystemDisplayMode getDisplayMode() const { return m_displayMode; }

		inline void addComputeShader(Shared<ComputeShader> cs) { m_shaders.push_back(cs); };

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

		//void Update(Timestep ts);//execute all linked compute shader
		//void execute(Shared<ComputeShader> step, bool autoBind = true); //execute compute shader once

		inline static Shared<deprecated_ParticleSystem> create(std::string name, GLsizeiptr instances) {
			return createShared<deprecated_ParticleSystem>(name, instances);
		};

		inline void addStorageBuffer(Shared<GenericBufferObject> buffer) {
			m_buffers.push_back(buffer);
			if (m_shaders.size() == 0) Console::info("ParticleSystem") << "Shader list is empty. Load particle system shader before adding buffer to enable automatic shader attach or attach them manually" << Console::endl;
			for (Shared<ComputeShader> shader : m_shaders) {
				shader->attach(*buffer);
			}
		}

	private:
		//Buffers & Compute Shaders
		std::vector<Shared<GenericBufferObject>> m_buffers; //Buffer to store the particle 
		
	};

	typedef Shared<deprecated_ParticleSystem> deprecated_ParticleSystem_Ptr;

}
