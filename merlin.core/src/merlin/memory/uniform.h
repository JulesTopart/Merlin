#pragma once
#include "merlin/shaders/shaderBase.h"
#include <vector>
namespace Merlin {

	template <class T>
	class Uniform {
	public:
		Uniform(const std::string& name, T initValue) : m_name(name), m_data(initValue){}

		inline T& value() {
			return m_data;
		}

		void sync(ShaderBase& shader);

	private:
		T m_data;
		std::string m_name;
	};

	template<>
	inline void Uniform<int>::sync(ShaderBase& shader) {
		shader.setInt(m_name, m_data);
	}

	template<>
	inline void Uniform<float>::sync(ShaderBase& shader) {
		shader.setFloat(m_name, m_data);
	}

	template<>
	inline void Uniform<double>::sync(ShaderBase& shader) {
		shader.setDouble(m_name, m_data);
	}

	template<>
	inline void Uniform<GLuint>::sync(ShaderBase& shader) {
		shader.setUInt(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::vec2>::sync(ShaderBase& shader) {
		shader.setVec2(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::ivec2>::sync(ShaderBase& shader) {
		shader.setIVec2(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::uvec2>::sync(ShaderBase& shader) {
		shader.setUVec2(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::vec3>::sync(ShaderBase& shader) {
		shader.setVec3(m_name, m_data);
	}
	template<>
	inline void Uniform<glm::ivec3>::sync(ShaderBase& shader) {
		shader.setIVec3(m_name, m_data);
	}
	template<>
	inline void Uniform<glm::uvec3>::sync(ShaderBase& shader) {
		shader.setUVec3(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::vec4>::sync(ShaderBase& shader) {
		shader.setVec4(m_name, m_data);
	}
	template<>
	inline void Uniform<glm::ivec4>::sync(ShaderBase& shader) {
		shader.setIVec4(m_name, m_data);
	}
	template<>
	inline void Uniform<glm::uvec4>::sync(ShaderBase& shader) {
		shader.setUVec4(m_name, m_data);
	}

	template<>
	inline void Uniform<glm::mat4>::sync(ShaderBase& shader) {
		shader.setMat4(m_name, m_data);
	}

}