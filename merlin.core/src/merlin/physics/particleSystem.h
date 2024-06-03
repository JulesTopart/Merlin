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
		//SPRITE,
		POINT_SPRITE,
		POINT_SPRITE_SHADED
	};

	class ParticleSystem : public RenderableObject {
	public:
		ParticleSystem(const std::string& name, size_t count);
		void draw() const; //draw the mesh
		void setInstancesCount(size_t count);

		GenericBufferObject_Ptr getField(const std::string& name);
		
		void addField(const std::string& name, GenericBufferObject_Ptr buf);
		bool hasField(const std::string& name);

		void addProgram(ComputeShader_Ptr program);
		void addProgram(const std::string& name, ComputeShader_Ptr program);
		bool hasProgram(const std::string& name);
		
		void setShader(Shader_Ptr shader);
		void setShader(const std::string& shader);
		Shader_Ptr getShader() const;
		bool hasShader() const;

		void link(const std::string& shader, const std::string& field);

		inline void setMesh(Shared<Mesh> geometry) { m_geometry = geometry; }
		inline Shared<Mesh> getMesh() const { return m_geometry; }

		inline void setDisplayMode(ParticleSystemDisplayMode mode) { m_displayMode = mode; }
		inline ParticleSystemDisplayMode getDisplayMode() const { return m_displayMode; }

		//templates
		template<typename T>
		void addField(const std::string& name);

		static Shared<ParticleSystem> create(const std::string&, size_t count);



	protected:
		//Rendering
		Shader_Ptr m_shader = nullptr;
		Mesh_Ptr m_geometry = nullptr;
		size_t m_instancesCount = 1;
		ParticleSystemDisplayMode m_displayMode = ParticleSystemDisplayMode::POINT_SPRITE;

		//Simulation
		std::map<std::string, ComputeShader_Ptr> m_programs; //Shader to compute the particle position
		std::map<std::string, GenericBufferObject_Ptr> m_fields; //Buffer to store particles fields
		std::map<std::string, std::vector<std::string>> m_links;
	};

	typedef Shared<ParticleSystem> ParticleSystem_Ptr;

	template<typename T>
	void ParticleSystem::addField(const std::string& name) {
		if(hasField(name)) {
			Console::warn("ParticleSystem") << name << "has been overwritten" << Console::endl;
		}
		SSBO_Ptr<T> f = SSBO<T>::create(name, m_instancesCount);
		m_fields[name] = f;
	}

}
