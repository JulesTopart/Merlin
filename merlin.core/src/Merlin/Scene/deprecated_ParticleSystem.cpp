#include "glpch.h"
#include "GL.h"
#include "deprecated_ParticleSystem.h"
#include "Merlin/Memory/IndexBuffer.h"


namespace Merlin::Tensor {

	deprecated_GenericParticleSystem::deprecated_GenericParticleSystem() : RenderableObject() {}
	deprecated_GenericParticleSystem::deprecated_GenericParticleSystem(const std::string& name, GLsizeiptr instances) : RenderableObject(name, ObjectType::PARTICLESYSTEM), m_instancesCount(instances){}

	void deprecated_GenericParticleSystem::Draw(const Shader& shader) const {
		m_geometry->DrawInstanced(m_instancesCount);
	}

}