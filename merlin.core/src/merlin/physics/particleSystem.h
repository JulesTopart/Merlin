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

	enum class ParticleSystemDisplayMode {
		MESH,
		SPRITE,
		POINT_SPRITE,
		POINT_SPRITE_SHADED
	};

	class ParticleSystem : public RenderableObject {
	public:
		ParticleSystem(std::string name, GLsizeiptr count);
		void draw(const Shader& shader) const; //draw the mesh
		void setInstancesCount(GLsizeiptr count);

		GenericBufferObject_Ptr getFieldBuffer(const std::string& name);
		void addField(GenericBufferObject_Ptr buf);

		bool hasField(std::string name);

		void addShader(GenericShader_Ptr shader);
		void addShader(std::string name, GenericShader_Ptr shader);

		bool hasShader(std::string name);

		inline void setMesh(Shared<Mesh> geometry) { m_geometry = geometry; }
		inline Shared<Mesh> getMesh() const { return m_geometry; }

		inline void setDisplayMode(ParticleSystemDisplayMode mode) { m_displayMode = mode; }
		inline ParticleSystemDisplayMode getDisplayMode() const { return m_displayMode; }

		static Shared<ParticleSystem> Create(std::string name, GLsizeiptr count);



		//templates
		template<typename T>
		void addField(const std::string& name);

		template<typename T>
		void writeField(const std::string& name, std::vector<T> data);








	protected:
		//Rendering
		Shader_Ptr m_shader = nullptr;
		Shared<Mesh> m_geometry = nullptr;
		GLsizeiptr m_instancesCount = 1;
		ParticleSystemDisplayMode m_displayMode = ParticleSystemDisplayMode::POINT_SPRITE;

		//Simulation
		std::map<std::string, ComputeShader> m_programs; //Shader to compute the particle position
		std::map<std::string, GenericBufferObject_Ptr> m_fields; //Buffer to store particles fields

	};


	typedef Shared<ParticleSystem> ParticleSystem_Ptr;

}
