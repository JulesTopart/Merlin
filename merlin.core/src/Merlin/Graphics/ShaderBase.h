#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/BufferObject.h"

#include <string>
#include <memory>
#include <glm/glm.hpp>

namespace Merlin::Graphics {
	class Merlin::Memory::GenericBufferObject;

	class ShaderBase {
	public:
		ShaderBase();
		ShaderBase(std::string name);
		virtual ~ShaderBase();

		virtual void Use() const;
		virtual void Delete();

		GLuint GetUniformLocation(const char*) const;

		void SetInt(const std::string name, GLint value) const;
		void SetUInt(const std::string name, GLuint value) const;
		void SetFloat(const std::string name, GLfloat value) const;
		void SetDouble(const std::string name, GLdouble value) const;
		void SetVec2(const std::string name, glm::vec2 value) const;
		void SetVec3(const std::string name, glm::vec3 value) const;
		void SetVec4(const std::string name, glm::vec4 value) const;
		void SetMat4(const std::string name, glm::mat4 mat) const;
		void SetIntArray(const std::string name, GLint* values, uint32_t count) const;

		void Attach(Memory::GenericBufferObject& buf, GLuint bindingPoint = -1);

		inline const GLuint id() const { return m_programID; }
		inline void SetID(GLuint _id_) { m_programID = _id_; };

		inline const std::string name() const { return m_name; }
		inline const bool IsCompiled() const { return m_compiled; }

		//static std::shared_ptr<ShaderBase> Create(const std::string& name);
		static std::string ReadSrc(const std::string& filename);

	protected:
		std::string m_name;
		bool m_compiled = false;
		GLuint m_attachedBuffers = 0;

		static int shader_instances;

	private:
		GLuint m_programID = 0;
	};

	template<class T>
	class UniformObject {
	public:
		UniformObject(const std::string& name) : m_name(name), value() {};
		void Sync(Merlin::Graphics::ShaderBase& shader) const;

		T value;
	protected:
		std::string m_name;
	};

	template<class T>
	inline void UniformObject<T>::Sync(Merlin::Graphics::ShaderBase& shader) const {
		Console::error("UniformObject") << "This uniform type is not supported yet in this class" << Console::endl;
	}

	template<>
	inline void UniformObject<int>::Sync(Merlin::Graphics::ShaderBase& shader) const {
		shader.SetInt(m_name, value);
	}

	template<>
	inline void UniformObject<unsigned int>::Sync(Merlin::Graphics::ShaderBase& shader) const {
		shader.SetUInt(m_name, value);
	}

	template<>
	inline void UniformObject<float>::Sync(Merlin::Graphics::ShaderBase& shader) const {
		shader.SetFloat(m_name, value);
	}

	template<>
	inline void UniformObject<glm::vec2>::Sync(Merlin::Graphics::ShaderBase& shader) const {
		shader.SetVec2(m_name, value);
	}

	template<>
	inline void UniformObject<glm::vec3>::Sync(Merlin::Graphics::ShaderBase& shader) const {
		shader.SetVec3(m_name, value);
	}

	template<>
	inline void UniformObject<glm::vec4>::Sync(Merlin::Graphics::ShaderBase& shader) const {
		shader.SetVec4(m_name, value);
	}

	template<>
	inline void UniformObject<glm::mat4>::Sync(Merlin::Graphics::ShaderBase& shader) const {
		shader.SetMat4(m_name, value);
	}

	template<>
	inline void UniformObject<std::vector<int>>::Sync(Merlin::Graphics::ShaderBase& shader) const {
		shader.SetIntArray(m_name, (GLint*)(value.data()), value.size());
	}
}