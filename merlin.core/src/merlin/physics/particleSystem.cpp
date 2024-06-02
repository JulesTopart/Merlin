#include "glpch.h"
#include "particleSystem.h"

namespace Merlin{
	Shared<ParticleSystem> ParticleSystem::Create(std::string name, GLsizeiptr count){
		return std::make_shared<ParticleSystem>(name, count);
	}


	ParticleSystem::ParticleSystem(std::string name, GLsizeiptr count) : RenderableObject(name), m_instancesCount(count) {
	}

	void ParticleSystem::draw(const Shader& shader) const { m_geometry->drawInstanced(m_instancesCount); } //draw the mesh

	void ParticleSystem::setInstancesCount(GLsizeiptr count) {
		m_instancesCount = count;
	}

	template<typename T>
	void ParticleSystem::addField(SSBO_Ptr<T> buf) {
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
	void ParticleSystem::addField(const std::string& name) {
		if (hasField(name)) {
			Console::error("ParticleSystem") << "This field already exist." << Console::endl;
			return;
		}
		SSBO_Ptr<T> f = SSBO<T>::create(name, m_instancesCount);
		m_fields[name] = f;
		f.setBindingPoint(m_fields.size() - 1);

		if (m_shaders.size() == 0) Console::info("ParticleSystem") << "Shader list is empty. Load particle system shader before adding buffer to enable automatic shader attach or attach them manually" << Console::endl;
		for (GenericShader_Ptr shader : m_shaders) {
			shader->attach(*f);
		}
	}

	template<typename T>
	SSBO_Ptr<T> ParticleSystem::getField(const std::string& name) {
		if (!hasField(name)) {
			Console::error("ParticleSystem") << "Field " << name << " does not exist." << Console::endl;
			return nullptr;
		}
		return dynamic_cast<SSBO_Ptr<T>>(m_fields.at(name).get());
	}

	GenericBufferObject_Ptr ParticleSystem::getFieldBuffer(const std::string& name) {
		if (!hasField(name)) {
			Console::error("ParticleSystem") << "Field " << name << " does not exist." << Console::endl;
			return nullptr;
		}
		return m_fields.at(name);
	}

	template<typename T>
	void ParticleSystem::writeField(const std::string& name, std::vector<T> data) {
		if (!hasField(name)) {
			Console::error("ParticleSystem") << "Field " << name << " does not exist." << Console::endl;
			return;
		}
		SSBO_Ptr f = dynamic_cast<SSBO_Ptr<T>>(m_fields.at(name).get());
		f.write(data);
	}

	bool ParticleSystem::hasField(std::string name) {
		return m_fields.find(name) != m_fields.end();
	}

	void ParticleSystem::addShader(GenericShader_Ptr shader) {
		if (hasField(shader->name())) {
			Console::warn("ParticleSystem") << "Shader " << shader->name() << "already exist and has been overwritten" << Console::endl;
		}
		m_shaders[shader->name()] = shader;
	}

	void ParticleSystem::addShader(std::string name, GenericShader_Ptr shader) {
		if (hasField(name)) {
			Console::warn("ParticleSystem") << "Shader " << name << "already exist and has been overwritten" << Console::endl;
		}
		m_shaders[name] = shader;
	}

	bool ParticleSystem::hasShader(std::string name) {
		return m_fields.find(name) != m_fields.end();
	}



}