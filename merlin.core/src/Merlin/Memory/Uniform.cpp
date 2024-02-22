#include "glpch.h"
#include "Uniform.h"

namespace Merlin {

	template<>
	void Uniform<int>::Sync(ShaderBase& shader) {
		shader.SetInt(m_name, m_data);
	}

	template<>
	void Uniform<float>::Sync(ShaderBase& shader) {
		shader.SetFloat(m_name, m_data);
	}

	template<>
	void Uniform<double>::Sync(ShaderBase& shader) {
		shader.SetDouble(m_name, m_data);
	}

	template<>
	void Uniform<GLuint>::Sync(ShaderBase& shader) {
		shader.SetUInt(m_name, m_data);
	}

	template<>
	void Uniform<glm::vec2>::Sync(ShaderBase& shader) {
		shader.SetVec2(m_name, m_data);
	}

	template<>
	void Uniform<glm::vec3>::Sync(ShaderBase& shader) {
		shader.SetVec3(m_name, m_data);
	}

	template<>
	void Uniform<glm::vec4>::Sync(ShaderBase& shader) {
		shader.SetVec4(m_name, m_data);
	}

	template<>
	void Uniform<glm::mat4>::Sync(ShaderBase& shader) {
		shader.SetMat4(m_name, m_data);
	}

}