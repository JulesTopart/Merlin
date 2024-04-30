#include "glpch.h"
#include "deprecated_particleSystem.h"
#include "merlin/memory/indexBuffer.h"


namespace Merlin {

	deprecated_GenericParticleSystem::deprecated_GenericParticleSystem() : RenderableObject() {}
	deprecated_GenericParticleSystem::deprecated_GenericParticleSystem(const std::string& name, GLsizeiptr instances) : RenderableObject(name, ObjectType::PARTICLESYSTEM), m_instancesCount(instances){}

	void deprecated_GenericParticleSystem::draw(const Shader& shader) const {
		m_geometry->drawInstanced(m_instancesCount);
	}

}