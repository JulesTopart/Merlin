#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Core/Log.h"
#include "Merlin/Memory/BufferObject.h"

#include <string>
#include <memory>

namespace Merlin::Graphics {

	class ShaderBase {
	public:
		ShaderBase(std::string name = "default");
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

		void Attach(GenericBufferObject& buf);
		void Attach(GenericBufferObject& buf, GLuint bindingPoint);

		inline const GLuint id() const { return m_programID; }
		inline void SetID(GLuint _id_) { m_programID = _id_; };

		inline const std::string name() const { return m_name; }
		inline const bool IsCompiled() const { return m_compiled; }

		//static std::shared_ptr<ShaderBase> Create(const std::string& name);
		static std::string ReadSrc(const std::string& filename);

	protected:
		std::string m_name;
		bool m_compiled = false;
		unsigned int m_attachedBuffers = 0;


	private:
		GLuint m_programID = 0;
	};
}