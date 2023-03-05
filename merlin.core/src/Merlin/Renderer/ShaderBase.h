#pragma once
#include "Merlin/Core/Core.h"

#include <string>
#include <memory>

namespace Merlin::Renderer {
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

		inline const GLuint id() const { return ProgramID; }
		inline void SetID(GLuint _id_) { ProgramID = _id_; };

		inline const std::string name() const { return _name; }
		inline const bool compiled() { return _compiled; }

		static std::shared_ptr<ShaderBase> Create(const std::string& name);
		static std::string ReadSrc(const std::string filename);

	protected:

		std::string _name;
		bool _compiled = false;

	private:
		GLuint ProgramID = 0;
	};

	class ShaderLibrary
	{
	public:
		void Add(const std::string& name, const std::shared_ptr<ShaderBase>& shader);
		void Add(const std::shared_ptr<ShaderBase>& shader);
		std::shared_ptr<ShaderBase> Load(const std::string& filepath);
		std::shared_ptr<ShaderBase> Get(const std::string& name);

		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, std::shared_ptr<ShaderBase>> m_Shaders;
	};
}