#include "glpch.h"
#include "particleSystem.h"
#include "merlin/utils/primitives.h"
#include "merlin/graphics/ressourceManager.h"

namespace Merlin{

	Shared<ParticleSystem> ParticleSystem::create(const std::string& name, size_t count) {
		return std::make_shared<ParticleSystem>(name, count);
	}

	ParticleSystem::ParticleSystem(const std::string& name, size_t count) : RenderableObject(name), m_instancesCount(count) {
		m_geometry = Merlin::Primitives::createPoint();
	}

	void ParticleSystem::draw() const { 
		
		switch (m_displayMode) {
		case ParticleSystemDisplayMode::MESH:
			if(m_geometry) m_geometry->drawInstanced(m_instancesCount);
			break;
		case ParticleSystemDisplayMode::POINT_SPRITE :
			glEnable(GL_PROGRAM_POINT_SIZE);
			if (m_geometry) m_geometry->drawInstanced(m_instancesCount);
			glDisable(GL_PROGRAM_POINT_SIZE);
			break;
		case ParticleSystemDisplayMode::POINT_SPRITE_SHADED:
			glEnable(GL_PROGRAM_POINT_SIZE);
			glEnable(0x8861);//Point shading
			if (m_geometry) m_geometry->drawInstanced(m_instancesCount);
			glDisable(GL_PROGRAM_POINT_SIZE);
			glDisable(0x8861);
			break;
		}
	} //draw the mesh

	void ParticleSystem::setInstancesCount(size_t count) {
		Console::warn() << "(Performance) Buffers of the particle system are being resized dynamically" << Console::endl;
		m_instancesCount = count; 
		for (auto field : m_fields) {
			field.second->resize(count);
		}
	}

	GenericBufferObject_Ptr ParticleSystem::getField(const std::string& name) {
		if (hasField(name)) {
			return m_fields[name];
		}
		else {
			Console::error("ParticleSystem") << "Unknown field " << name << Console::endl;
			return nullptr;
		}
	}


	void ParticleSystem::addField(const std::string& name, GenericBufferObject_Ptr field) {
		if (hasField(name)) {
			Console::warn("ParticleSystem") << name << "has been overwritten" << Console::endl;
		}
		m_fields[name] = field;
	}
	bool ParticleSystem::hasField(const std::string& name) {
		return m_fields.find(name) != m_fields.end();
	}

	void ParticleSystem::addProgram(ComputeShader_Ptr program) {
		if (hasProgram(program->name())) {
			Console::warn("ParticleSystem") << program->name() << "has been overwritten" << Console::endl;
		}
		m_programs[program->name()] = program;
	}
	void ParticleSystem::addProgram(const std::string& name, ComputeShader_Ptr program) {
		if (hasProgram(name)) {
			Console::warn("ParticleSystem") << name << "has been overwritten" << Console::endl;
		}
		m_programs[name] = program;
	}
	bool ParticleSystem::hasProgram(const std::string& name) {
		return m_programs.find(name) != m_programs.end();
	}

	void ParticleSystem::setShader(Shader_Ptr shader) {
		m_shader = shader;
	}
	void ParticleSystem::setShader(const std::string& shader) {
		m_shader = ShaderLibrary::instance().get(shader);
	}
	bool ParticleSystem::hasShader() const{
		return m_shader != nullptr;
	}
	Shader_Ptr ParticleSystem::getShader() const{
		return m_shader;
	}

	void ParticleSystem::link(const std::string& shader, const std::string& field) {
		if (m_links.find(shader) != m_links.end()) {
			m_links[shader] = std::vector<std::string>();
		}
		m_links[shader].push_back(field);
	}


}