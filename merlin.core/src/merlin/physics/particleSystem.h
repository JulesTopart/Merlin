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
		ParticleSystem(std::string name, GLsizeiptr count) : RenderableObject(name), m_instancesCount(count){
		}

		inline void draw(const Shader& shader) const { m_geometry->drawInstanced(m_instancesCount); } //draw the mesh

		inline void setInstancesCount(GLsizeiptr count) {
			m_instancesCount = count;	
		}
		template<typename T>
		void addField(SSBO_Ptr<T> buf) {
			if (hasField(buf->name())) {
				Console::error("ParticleSystem") << "This field already exist." << Console::endl;
				return;
			}
			m_fields[buf->name()] = buf;
			buf->setBindingPoint(m_fields.size() - 1);
			if (m_shaders.size() == 0) Console::info("ParticleSystem") << "Shader list is empty. Load particle system shader before adding buffer to enable automatic shader attach or attach them manually" << Console::endl;
			for (GenericShader_Ptr shader : m_shaders) {
				shader->attach(*buf);
			}
		}

		template<typename T>
		void addField(const std::string& name) {
			if (hasField(name)) {
				Console::error("ParticleSystem") << "This field already exist." << Console::endl;
				return;
			}
			SSBO_Ptr<T> f = SSBO<T>::create(name, m_instancesCount);
			m_fields[name] = f;
			f.setBindingPoint(m_fields.size()-1);

			if (m_shaders.size() == 0) Console::info("ParticleSystem") << "Shader list is empty. Load particle system shader before adding buffer to enable automatic shader attach or attach them manually" << Console::endl;
			for (GenericShader_Ptr shader : m_shaders) {
				shader->attach(*f);
			}
		}

		template<typename T>
		SSBO_Ptr<T> getField(const std::string& name) {
			if (!hasField(name)) {
				Console::error("ParticleSystem") << "Field " << name << " does not exist." << Console::endl;
				return nullptr;
			}
			return dynamic_cast<SSBO_Ptr<T>>(m_fields.at(name).get());
		}

		GenericBufferObject_Ptr getFieldBuffer(const std::string& name) {
			if (!hasField(name)) {
				Console::error("ParticleSystem") << "Field " << name << " does not exist." << Console::endl;
				return nullptr;
			}
			return m_fields.at(name);
		}

		template<typename T>
		void writeField(const std::string& name, std::vector<T> data) {
			if (!hasField(name)) {
				Console::error("ParticleSystem") << "Field " << name << " does not exist." << Console::endl;
				return;
			}
			SSBO_Ptr f = dynamic_cast<SSBO_Ptr<T>>(m_fields.at(name).get());
			f.write(data);
		}
		
		bool hasField(std::string name) {
			return m_fields.find(name) != m_fields.end();
		}

		void addShader(GenericShader_Ptr shader) {
			if (hasField(shader->name())) {
				Console::warn("ParticleSystem") << "Shader " << shader->name() << "already exist and has been overwritten" << Console::endl;
			}
			m_shaders[shader->name()] = shader;
		}	

		void addShader(std::string name, GenericShader_Ptr shader) {
			if (hasField(name)) {
				Console::warn("ParticleSystem") << "Shader " << name << "already exist and has been overwritten" << Console::endl;
			}
			m_shaders[name] = shader;
		}

		bool hasShader(std::string name) {
			return m_fields.find(name) != m_fields.end();
		}

		inline void setMesh(Shared<Mesh> geometry) { m_geometry = geometry; }
		inline Shared<Mesh> getMesh() const { return m_geometry; }

		inline void setDisplayMode(ParticleSystemDisplayMode mode) { m_displayMode = mode; }
		inline ParticleSystemDisplayMode getDisplayMode() const { return m_displayMode; }

		static Shared<ParticleSystem> Create(std::string name, GLsizeiptr count);
	protected:
		//Rendering
		Shared<Mesh> m_geometry = nullptr;
		GLsizeiptr m_instancesCount = 1;
		ParticleSystemDisplayMode m_displayMode = ParticleSystemDisplayMode::POINT_SPRITE;

		//Simulation
		std::map<std::string, GenericShader_Ptr> m_shaders; //Shader to compute the particle position
		std::map<std::string, GenericBufferObject_Ptr> m_fields; //Buffer to store particles fields

	};


	typedef Shared<ParticleSystem> ParticleSystem_Ptr;

}
