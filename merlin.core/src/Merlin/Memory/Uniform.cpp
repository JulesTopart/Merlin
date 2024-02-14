#include "glpch.h"
#include "Uniform.h"

namespace Merlin::Memory {

	template<>
	void Uniform<int>::Sync(ShaderBase& shader) {
		if (HasChanged() || needRefresh) {
			//shader->Use();
			shader.SetInt(m_name, m_data);
			m_dataCopy = m_data;
			needRefresh = false;
		}
	}

	template<>
	void Uniform<float>::Sync(ShaderBase& shader) {
		if (HasChanged() || needRefresh) {
			//shader->Use();
			shader.SetFloat(m_name, m_data);
			m_dataCopy = m_data;
			needRefresh = false;
		}
	}

	template<>
	void Uniform<double>::Sync(ShaderBase& shader) {
		if (HasChanged() || needRefresh) {
			//shader->Use();
			shader.SetDouble(m_name, m_data);
			m_dataCopy = m_data;
			needRefresh = false;
		}
	}

	template<>
	void Uniform<GLuint>::Sync(ShaderBase& shader) {
		if (HasChanged() || needRefresh) {
			//shader->Use();
			shader.SetUInt(m_name, m_data);
			m_dataCopy = m_data;
			needRefresh = false;
		}
	}

	template<>
	void Uniform<glm::vec2>::Sync(ShaderBase& shader) {
		if (HasChanged() || needRefresh) {
			//shader->Use();
			shader.SetVec2(m_name, m_data);
			m_dataCopy = m_data;
			needRefresh = false;
		}
	}

	template<>
	void Uniform<glm::vec3>::Sync(ShaderBase& shader) {
		if (HasChanged() || needRefresh) {
			//shader->Use();
			shader.SetVec3(m_name, m_data);
			m_dataCopy = m_data;
			needRefresh = false;
		}
	}

	template<>
	void Uniform<glm::vec4>::Sync(ShaderBase& shader) {
		if (HasChanged() || needRefresh) {
			//shader->Use();
			shader.SetVec4(m_name, m_data);
			m_dataCopy = m_data;
			needRefresh = false;
		}
	}

	template<>
	void Uniform<glm::mat4>::Sync(ShaderBase& shader) {
		if (HasChanged() || needRefresh) {
			//shader->Use();
			shader.SetMat4(m_name, m_data);
			m_dataCopy = m_data;
			needRefresh = false;
		}
	}

}