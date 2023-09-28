#include "glpch.h"
#include "GL.h"
#include "ParticleSystem.h"
#include "Merlin/Memory/IndexBuffer.h"


namespace Merlin::Tensor {

    GenericParticleSystem::GenericParticleSystem() : RenderableObject() {}
    GenericParticleSystem::GenericParticleSystem(const std::string& name, GLsizeiptr instances) : RenderableObject(name, ObjectType::PARTICLESYSTEM), m_instancesCount(instances){}

	void GenericParticleSystem::Draw(const Shader& shader) const {
		m_geometry->DrawInstanced(m_instancesCount);
	}


}