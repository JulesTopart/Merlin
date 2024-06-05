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
		if (count == m_instancesCount) return;
		if(m_fields.size() != 0) Console::warn() << "(Performance) Buffers of the particle system are being resized dynamically" << Console::endl;
		m_instancesCount = count; 
		for (auto field : m_fields) {
			field.second->reserve(count);
		}
	}

	GenericBufferObject_Ptr ParticleSystem::getField(const std::string& name) const {
		if (hasField(name)) {
			return m_fields.at(name);
		}
		else {
			Console::error("ParticleSystem") << "Unknown field " << name << Console::endl;
			return nullptr;
		}
	}


	void ParticleSystem::addField(GenericBufferObject_Ptr field) {
		if (hasField(field->name())) {
			Console::warn("ParticleSystem") << field->name() << "has been overwritten" << Console::endl;
		}
		m_fields[field->name()] = field;
		if (hasLink(m_currentProgram)) {
			link(m_currentProgram, field->name());
		}
	}
	bool ParticleSystem::hasField(const std::string& name) const{
		return m_fields.find(name) != m_fields.end();
	}

	void ParticleSystem::writeField(const std::string& name, void* data){
		if (hasField(name)) {
			m_fields[name]->bind();
			m_fields[name]->writeRaw(m_fields[name]->size(), data);
		}
	}

	void ParticleSystem::addProgram(ComputeShader_Ptr program) {
		if (hasProgram(program->name())) {
			Console::warn("ParticleSystem") << program->name() << "has been overwritten" << Console::endl;
		}
		m_links[program->name()] = std::vector<std::string>();
		m_programs[program->name()] = program;
		m_currentProgram = program->name();

		GLuint pWkgSize = 64; //Number of thread per workgroup
		GLuint pWkgCount = (m_instancesCount + pWkgSize - 1) / pWkgSize; //Total number of workgroup needed
		program->SetWorkgroupLayout(pWkgCount);
	}

	bool ParticleSystem::hasProgram(const std::string& name) const {
		return m_programs.find(name) != m_programs.end();
	}

	void ParticleSystem::setShader(Shader_Ptr shader) {
		if (m_shader)
			m_links.erase(m_shader->name());
		m_shader = shader;
		m_currentProgram = m_shader->name();
		if (hasField("position_buffer")) { //automatically link position_buffer
			link(m_shader->name(), "position_buffer");
		}
	}

	bool ParticleSystem::hasShader() const{
		return m_shader != nullptr;
	}
	Shader_Ptr ParticleSystem::getShader() const{
		return m_shader;
	}

	void ParticleSystem::link(const std::string& shader, const std::string& field) {
		if (!hasLink(shader)) {
			m_links[shader] = std::vector<std::string>();
		}
		m_links[shader].push_back(field);
	}

	bool ParticleSystem::hasLink(const std::string& shader) const{
		return m_links.find(shader) != m_links.end();
	}

	void ParticleSystem::solveLink(Shared<ShaderBase> shader) {
		if (hasLink(shader->name())) {
			for (auto& entry : m_links[shader->name()]) {
				if(hasField(entry))
					shader->attach(*getField(entry));
				else
					Console::error("ParticleSystem") << entry << " is not registered as field in the particle system" << Console::endl;
				
			}
		}
		else {
			Console::error("ParticleSystem") << shader->name() << " is not registered in the particle system" << Console::endl;
		}
		
	}


}