#include "glpch.h"
#include "ShaderBase.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace Merlin::Renderer {

	ShaderBase::ShaderBase(std::string name) {
		_name = name;
	}

	ShaderBase::~ShaderBase() {
		Delete();
	}

	void ShaderBase::Delete() {
		LOG_TRACE("ShaderBase") << "Shader " << ProgramID << " deleted. " << Console::endl;
		if (_compiled != 0) {
			glDeleteProgram(ProgramID);
			ProgramID = 0;
		}
	}

	void ShaderBase::Use() {
		LOG_TRACE("Shader") << "Using program : " << ProgramID << Console::endl;
		glUseProgram(ProgramID);
	}

	GLuint ShaderBase::GetUniformLocation(const char* uniform) {
		GLuint uniLoc = glGetUniformLocation(ProgramID, uniform);
		if (uniLoc == -1) LOG_ERROR("Shader") << "Invalid Uniform name : " << uniform << ", (or wrong binded shader)" << Console::endl;
		return uniLoc;
	}

	void ShaderBase::SetUInt(const std::string name, GLuint value) {
		int uniformLocation = GetUniformLocation(name.c_str());
		glUniform1ui(uniformLocation, value);
	}

	void ShaderBase::SetInt(const std::string name, GLint value) {
		int uniformLocation = GetUniformLocation(name.c_str());
		glUniform1i(uniformLocation, value);
	}

	void ShaderBase::SetFloat(const std::string name, GLfloat value) {
		int uniformLocation = GetUniformLocation(name.c_str());
		glUniform1f(uniformLocation, value);
	}

	void ShaderBase::SetMatrix4f(const std::string name, glm::mat4 mat) {
		glUniformMatrix4fv(GetUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
	}

	void ShaderBase::SetUniform4f(const std::string name, glm::vec4 value) {
		glUniform4fv(GetUniformLocation(name.c_str()), 1, glm::value_ptr(value));
	}

	void ShaderBase::SetUniform3f(const std::string name, glm::vec3 value) {
		glUniform3fv(GetUniformLocation(name.c_str()), 1, glm::value_ptr(value));
	}

	void ShaderBase::SetUniform2f(const std::string name, glm::vec2 value) {
		glUniform2fv(GetUniformLocation(name.c_str()), 1, glm::value_ptr(value));
	}

	void ShaderBase::SetIntArray(const std::string name, GLint* values, uint32_t count) {
		glUniform2iv( GetUniformLocation(name.c_str()), count, values);
	}

	// Reads a text file and outputs a string with everything in the text file
	std::string ShaderBase::ReadSrc(const std::string filename)
	{
		std::ifstream in(filename, std::ios::binary);
		if (in) {
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
			return(contents);
		}
		else {
			LOG_ERROR("ShaderLoader") << "Can't read file " << filename << Console::endl;
		}
		throw(errno);
		return "error";
	}

	std::shared_ptr<ShaderBase> ShaderBase::Create(const std::string& name) {
		return std::make_shared<ShaderBase>(name);
	}
	



	void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<ShaderBase>& shader)
	{
		GLCORE_ASSERT(!Exists(name), "ShaderLibrary", "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const std::shared_ptr<ShaderBase>& shader)
	{
		auto& name = shader->name();
		Add(name, shader);
	}

	std::shared_ptr<ShaderBase> ShaderLibrary::Load(const std::string& name)
	{
		auto shader = ShaderBase::Create(name);
		Add(shader);
		return shader;
	}

	std::shared_ptr<ShaderBase> ShaderLibrary::Get(const std::string& name)
	{
		GLCORE_ASSERT(Exists(name), "ShaderLibrary", "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}

