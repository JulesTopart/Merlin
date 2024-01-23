#include "glpch.h"
#include "ShaderBase.h"
#include "Merlin/Core/Log.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <regex> // Include the <regex> header for regular expressions

#include <glm/gtc/type_ptr.hpp>

namespace Merlin::Graphics {

	int ShaderBase::shader_instances = 0;

	ShaderBase::ShaderBase() {
		shader_instances++;
		m_name = "default" + std::to_string(shader_instances);
	}

	ShaderBase::ShaderBase(std::string name) {
		shader_instances++;
		m_name = name;
	}

	ShaderBase::~ShaderBase() {
		Delete();
	}

	void ShaderBase::Delete() {
		LOG_TRACE("ShaderBase") << "Shader " << m_programID << " deleted. " << Console::endl;
		if (m_compiled != 0) {
			glDeleteProgram(m_programID);
			m_programID = 0;
		}
	}

	void ShaderBase::Use() const {
		//LOG_TRACE("Shader") << "Using program : " << m_programID << Console::endl;
		if (!IsCompiled()) {
			//LOG_ERROR("Shader") << "Failed to bind shader. Program is not compiled" << Console::endl;
			return;
		}
		glUseProgram(m_programID);
	}


	void ShaderBase::Attach(Memory::GenericBufferObject& buf, GLuint bindingPoint) {
		int block_index = glGetProgramResourceIndex(m_programID, GL_SHADER_STORAGE_BLOCK, buf.name().c_str());
		if (block_index == -1) Console::error("ShaderBase") << "Block " << buf.name() << " not found in shader '" << m_name << "'. Did you bind it properly ?" << Console::endl;
		else {
			if (bindingPoint == GLuint(-1)) bindingPoint = m_attachedBuffers++;
			m_attachedBuffers++;
			buf.Bind();
			Console::info("ShaderBase") << buf.name() << "( block index " << block_index << ") is now bound to " << name() << " using binding point " << bindingPoint << Console::endl;
			glShaderStorageBlockBinding(m_programID, block_index, bindingPoint);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, buf.id()); //Do this explicitly in your shader !
		}
	}

	//TODO : Bind the shaders automatically before setting uniforms.
	GLuint ShaderBase::GetUniformLocation(const char* uniform) const{
		if (!IsCompiled()) return 0;
		GLuint uniLoc = glGetUniformLocation(m_programID, uniform);
		if (uniLoc == -1) LOG_WARN("Shader") << "(" << m_name << ") Invalid Uniform name : " << uniform << ", (or wrong binded shader)" << Console::endl;
		return uniLoc;
	}

	void ShaderBase::SetUInt(const std::string name, GLuint value) const {
		if (!IsCompiled()) return;
		int uniformLocation = GetUniformLocation(name.c_str());
		glUniform1ui(uniformLocation, value);
	}

	void ShaderBase::SetInt(const std::string name, GLint value) const {
		if (!IsCompiled()) return;
		int uniformLocation = GetUniformLocation(name.c_str());
		glUniform1i(uniformLocation, value);
	}

	void ShaderBase::SetFloat(const std::string name, GLfloat value) const {
		if (!IsCompiled()) return;
		int uniformLocation = GetUniformLocation(name.c_str());
		glUniform1f(uniformLocation, value);
	}

	void ShaderBase::SetDouble(const std::string name, GLdouble value) const {
		if (!IsCompiled()) return;
		int uniformLocation = GetUniformLocation(name.c_str());
		glUniform1d(uniformLocation, value);
	}

	void ShaderBase::SetMat4(const std::string name, glm::mat4 mat) const {
		if (!IsCompiled()) return;
		glUniformMatrix4fv(GetUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
	}

	void ShaderBase::SetVec4(const std::string name, glm::vec4 value) const {
		if (!IsCompiled()) return;
		glUniform4fv(GetUniformLocation(name.c_str()), 1, glm::value_ptr(value));
	}

	void ShaderBase::SetVec3(const std::string name, glm::vec3 value) const {
		if (!IsCompiled()) return;
		glUniform3fv(GetUniformLocation(name.c_str()), 1, glm::value_ptr(value));
	}

	void ShaderBase::SetVec2(const std::string name, glm::vec2 value) const {
		if (!IsCompiled()) return;
		glUniform2fv(GetUniformLocation(name.c_str()), 1, glm::value_ptr(value));
	}

	void ShaderBase::SetIntArray(const std::string name, GLint* values, uint32_t count) const {
		if (!IsCompiled()) return;
		glUniform2iv( GetUniformLocation(name.c_str()), count, values);
	}

	std::string ShaderBase::ReadSrc(const std::string& filename)
	{
		std::ifstream in(filename, std::ios::binary);
		if (in) {
			std::string contents;
			in.seekg(0, std::ios::end);
			contents.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();

			// Find and replace include statements
			std::regex includeRegex("#include\\s+\"([^\"]+)\"");
			std::smatch includeMatch;

			std::string path = filename.substr(0, filename.find_last_of('/'));

			while (std::regex_search(contents, includeMatch, includeRegex)) {
				std::string includeFile = includeMatch[1].str();
				std::string absPath = path + "/" + includeFile;

				std::string includeContent = ReadSrc(absPath); // Recursive call to load included file's content
				contents = std::regex_replace(contents, includeRegex, includeContent, std::regex_constants::format_first_only);
			}

			return contents;
		}
		else {
			LOG_ERROR("ShaderLoader") << "Can't read file " << filename << Console::endl;
			//throw(errno);
			return "error";
		}
	}



	/*
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
	*/

	/*
	std::shared_ptr<ShaderBase> ShaderBase::Create(const std::string& name) {
		return std::make_shared<ShaderBase>(name);
	}
	*/


}

