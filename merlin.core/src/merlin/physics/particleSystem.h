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

		GenericBufferObject_Ptr getField(const std::string& name) const;
		GenericBufferObject_Ptr getBuffer(const std::string& name) const;
		
		void addField(GenericBufferObject_Ptr buf);
		void addBuffer(GenericBufferObject_Ptr buf);
		bool hasField(const std::string& name) const;
		bool hasBuffer(const std::string& name) const;
		void writeField(const std::string& name, void* data);

		void addProgram(ComputeShader_Ptr program);
		bool hasProgram(const std::string& name) const;
		
		void setShader(Shader_Ptr shader);
		inline void setShader(std::string shaderName) { m_shaderName = shaderName; }
		Shader_Ptr getShader() const;
		inline const std::string& getShaderName() const { return m_shaderName; }

		bool hasShader() const;

		inline void setMaterial(Shared<MaterialBase> material) { m_material = material; }
		inline void setMaterial(std::string materialName) { m_materialName = materialName; }
		inline bool hasMaterial() const { return m_material != nullptr; }
		inline const Shared<MaterialBase> getMaterial() const { return m_material; }
		inline const std::string& getMaterialName() const { return m_materialName; }


		void link(const std::string& shader, const std::string& field);
		void solveLink(Shared<ShaderBase>);
		bool hasLink(const std::string& name) const;


		inline void setMesh(Shared<Mesh> geometry) { m_geometry = geometry; }
		inline Shared<Mesh> getMesh() const { return m_geometry; }

		inline void setDisplayMode(ParticleSystemDisplayMode mode) { m_displayMode = mode; }
		inline ParticleSystemDisplayMode getDisplayMode() const { return m_displayMode; }

		//templates
		template<typename T>
		void addField(const std::string& name);

		template<typename T>
		void addBuffer(const std::string& name);

		static Shared<ParticleSystem> create(const std::string&, size_t count);



	protected:
		//Rendering
		std::string m_materialName = "default";
		Shared<MaterialBase> m_material = nullptr;

		Shader_Ptr m_shader = nullptr;
		std::string m_shaderName = "default";

		Mesh_Ptr m_geometry = nullptr;
		size_t m_instancesCount = 1;
		ParticleSystemDisplayMode m_displayMode = ParticleSystemDisplayMode::POINT_SPRITE;

		//Simulation
		std::map<std::string, ComputeShader_Ptr> m_programs; //Shader to compute the particle position
		std::map<std::string, GenericBufferObject_Ptr> m_fields; //Buffer to store particles fields
		std::map<std::string, GenericBufferObject_Ptr> m_buffers; //Buffer to store particles fields
		std::map<std::string, std::vector<std::string>> m_links;

		std::string m_currentProgram = "";
	};

	typedef Shared<ParticleSystem> ParticleSystem_Ptr;

	template<typename T>
	void ParticleSystem::addField(const std::string& name) {
		if(hasField(name)) {
			Console::warn("ParticleSystem") << name << "has been overwritten" << Console::endl;
		}
		SSBO_Ptr<T> f = SSBO<T>::create(name, m_instancesCount);
		m_fields[name] = f;

		if (hasLink(m_currentProgram)) {
			link(m_currentProgram, f->name());
		}
	}

}
