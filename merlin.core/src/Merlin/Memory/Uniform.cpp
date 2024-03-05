#include "glpch.h"
#include "Uniform.h"

namespace Merlin {

	template<>
	void Uniform<int>::sync(ShaderBase& shader) {
		shader.setInt(m_name, m_data);
	}

	template<>
	void Uniform<float>::sync(ShaderBase& shader) {
		shader.setFloat(m_name, m_data);
	}

	template<>
	void Uniform<double>::sync(ShaderBase& shader) {
		shader.setDouble(m_name, m_data);
	}

	template<>
	void Uniform<GLuint>::sync(ShaderBase& shader) {
		shader.setUInt(m_name, m_data);
	}

	template<>
	void Uniform<glm::vec2>::sync(ShaderBase& shader) {
		shader.setVec2(m_name, m_data);
	}

	template<>
	void Uniform<glm::vec3>::sync(ShaderBase& shader) {
		shader.setVec3(m_name, m_data);
	}

	template<>
	void Uniform<glm::vec4>::sync(ShaderBase& shader) {
		shader.setVec4(m_name, m_data);
	}

	template<>
	void Uniform<glm::mat4>::sync(ShaderBase& shader) {
		shader.setMat4(m_name, m_data);
	}

}