#pragma once
#include "Merlin/Graphics/ShaderBase.h"
#include <vector>
namespace Merlin::Memory {
	using namespace Merlin::Graphics;

	template <class T>
	class Uniform {
		Uniform(std::string name);
		Uniform(std::string name, T initValue);

		bool HasChanged();
		T& value();
		void Sync(ShaderBase* shader);

	private:
		bool m_hasChanged = false;
		T m_data, m_dataCopy;
		std::string m_name;
		std::vector<GLuint> m_programHandles;
	};

	template<class T>
	bool Uniform<T>::HasChanged() {
		return m_dataCopy != m_data;
	}

	template<class T>
	T& Uniform<T>::value() {
		return m_data;
	}

	template<>
	void Uniform<int>::Sync(ShaderBase* shader) {
		if (HasChanged()) {
			shader->Use();
			shader->SetInt(m_name, m_data);
			m_dataCopy = m_data;
		}
	}

	template<>
	void Uniform<float>::Sync(ShaderBase* shader) {
		if (HasChanged()) {
			shader->Use();
			shader->SetFloat(m_name, m_data);
			m_dataCopy = m_data;
		}
	}

	template<>
	void Uniform<double>::Sync(ShaderBase* shader) {
		if (HasChanged()) {
			shader->Use();
			shader->SetDouble(m_name, m_data);
			m_dataCopy = m_data;
		}
	}

	template<>
	void Uniform<std::vector<int>>::Sync(ShaderBase* shader) {
		if (HasChanged()) {
			shader->Use();
			shader->SetIntArray(m_name, m_data.data(), m_data.size());
			m_dataCopy = m_data;
		}
	}

	template<>
	void Uniform<GLuint>::Sync(ShaderBase* shader) {
		if (HasChanged()) {
			shader->Use();
			shader->SetUInt(m_name, m_data);
			m_dataCopy = m_data;
		}
	}

	template<>
	void Uniform<glm::vec2>::Sync(ShaderBase* shader) {
		if (HasChanged()) {
			shader->Use();
			shader->SetVec2(m_name, m_data);
			m_dataCopy = m_data;
		}
	}

	template<>
	void Uniform<glm::vec3>::Sync(ShaderBase* shader) {
		if (HasChanged()) {
			shader->Use();
			shader->SetVec3(m_name, m_data);
			m_dataCopy = m_data;
		}
	}

	template<>
	void Uniform<glm::vec4>::Sync(ShaderBase* shader) {
		if (HasChanged()) {
			shader->Use();
			shader->SetVec4(m_name, m_data);
			m_dataCopy = m_data;
		}
	}

	template<>
	void Uniform<glm::mat4>::Sync(ShaderBase* shader) {
		if (HasChanged()) {
			shader->Use();
			shader->SetMat4(m_name, m_data);
			m_dataCopy = m_data;
		}
	}
}