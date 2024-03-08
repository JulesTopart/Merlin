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

		template<typename T>
		void addField(const std::string& name) {
			SSBO_Ptr<T> f = SSBO<T>::create(name, m_instancesCount);
			m_fields[name] = f;
		}

		template<typename T>
		SSBO_Ptr<T> getField(std::string name) {
			if (hasField(name)) return dynamic_cast<SSBO_Ptr<T>>(m_fields.at(name).get());
			else return nullptr;
		}
		
		bool hasField(std::string name) {
			return m_fields.find(name) != m_fields.end();
		}

		template<typename T>
		void setField(std::string name, SSBO_Ptr<T> field) {
			if (hasField(name)) Console::warn("ParticleSystem") << "Field " << name << "already exist and has been overwritten" << Console::endl;
		}


	protected:
		//Rendering
		Shared<Mesh> m_geometry;
		GLsizeiptr m_instancesCount = 1;
		ParticleSystemDisplayMode m_displayMode = ParticleSystemDisplayMode::POINT_SPRITE;

		//Simulation
		std::vector<Shared<ShaderBase>> m_shaders; //Shader to compute the particle position
		std::map<std::string, GenericBufferObject_Ptr> m_fields; //Buffer to store particles fields

	};


}
