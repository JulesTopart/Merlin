#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/bufferObject.h"

#include <string>
#include <memory>
#include <glm/glm.hpp>

namespace Merlin {
	class Merlin::GenericBufferObject;


	enum class ShaderType {
		ABSTRACT,
		GRAPHICS, // Vertex, fragment or geometry
		COMPUTE_SHADER,
		STAGED_COMPUTE_SHADER
	};

	class ShaderBase {
	public:
		ShaderBase(ShaderType);
		ShaderBase(std::string name, ShaderType);
		virtual ~ShaderBase();

		virtual void use() const;
		virtual void destroy();

		GLuint getUniformLocation(const char*) const;

		void setInt(const std::string name, GLint value) const;
		void setUInt(const std::string name, GLuint value) const;
		void setFloat(const std::string name, GLfloat value) const;
		void setDouble(const std::string name, GLdouble value) const;
		void setVec2(const std::string name, glm::vec2 value) const;
		void setVec3(const std::string name, glm::vec3 value) const;
		void setVec4(const std::string name, glm::vec4 value) const;
		void setMat4(const std::string name, glm::mat4 mat) const;
		void setIntArray(const std::string name, GLint* values, uint32_t count) const;

		void attach(GenericBufferObject& buf);

		inline const GLuint id() const { return m_programID; }
		inline void setID(GLuint _id_) { m_programID = _id_; };

		inline const std::string name() const { return m_name; }
		inline const bool isCompiled() const { return m_compiled; }

		//static std::shared_ptr<ShaderBase> create(const std::string& name);
		static std::string readSrc(const std::string& filename);

	protected:
		std::string m_name;
		bool m_compiled = false;

		static int shader_instances;

	private:
		GLuint m_programID = 0;
		ShaderType m_type = ShaderType::ABSTRACT;
	};

	typedef Shared<ShaderBase> GenericShader_Ptr;

	template<class T>
	class UniformObject {
	public:
		UniformObject(const std::string& name) : m_name(name), value() {};
		void sync(Merlin::ShaderBase& shader) const;

		T value;
	protected:
		std::string m_name;
	};

	template<class T>
	inline void UniformObject<T>::sync(Merlin::ShaderBase& shader) const {
		Console::error("UniformObject") << "This uniform type is not supported yet in this class" << Console::endl;
	}

	template<>
	inline void UniformObject<int>::sync(Merlin::ShaderBase& shader) const {
		shader.setInt(m_name, value);
	}

	template<>
	inline void UniformObject<unsigned int>::sync(Merlin::ShaderBase& shader) const {
		shader.setUInt(m_name, value);
	}

	template<>
	inline void UniformObject<float>::sync(Merlin::ShaderBase& shader) const {
		shader.setFloat(m_name, value);
	}

	template<>
	inline void UniformObject<glm::vec2>::sync(Merlin::ShaderBase& shader) const {
		shader.setVec2(m_name, value);
	}

	template<>
	inline void UniformObject<glm::vec3>::sync(Merlin::ShaderBase& shader) const {
		shader.setVec3(m_name, value);
	}

	template<>
	inline void UniformObject<glm::vec4>::sync(Merlin::ShaderBase& shader) const {
		shader.setVec4(m_name, value);
	}

	template<>
	inline void UniformObject<glm::mat4>::sync(Merlin::ShaderBase& shader) const {
		shader.setMat4(m_name, value);
	}

	template<>
	inline void UniformObject<std::vector<int>>::sync(Merlin::ShaderBase& shader) const {
		shader.setIntArray(m_name, (GLint*)(value.data()), value.size());
	}
}