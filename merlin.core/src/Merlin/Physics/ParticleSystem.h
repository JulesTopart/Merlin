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
		ParticleSystem(std::string name) : RenderableObject(name) {

		}

		template<typename T>
		void addField(const std::string& name) {
			SSBO_Ptr<T> f = SSBO<T>::create(name, m_instancesCount);
			m_fields[name] = f;
		}

		template<typename T>
		SSBO_Ptr<T> getField(std::string name) {

		}

		template<typename T>
		void setField(std::string name, SSBO_Ptr<T> field) {

		}


	protected:
		//Rendering
		Shared<Mesh> m_geometry;
		GLsizeiptr m_instancesCount = 1;
		ParticleSystemDisplayMode m_displayMode = ParticleSystemDisplayMode::POINT_SPRITE;

		//Simulation
		std::vector<ComputeShader_Ptr> m_shaders; //Shader to compute the particle position
		std::map<std::string, GenericBufferObject_Ptr> m_fields; //Buffer to store particles fields

	};


}
