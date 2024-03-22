#include "glpch.h"
#include "ShaderBase.h"
#include "Merlin/Core/Log.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <regex> // Include the <regex> header for regular expressions

#include <glm/gtc/type_ptr.hpp>

namespace Merlin {

	int ShaderBase::shader_instances = 0;

	ShaderBase::ShaderBase(ShaderType type) {
		shader_instances++;
		m_type = type;
		m_name = "default" + std::to_string(shader_instances);
	}

	ShaderBase::ShaderBase(std::string name, ShaderType type) {
		shader_instances++;
		m_name = name;
		m_type = type;
	}

	ShaderBase::~ShaderBase() {
		destroy();
	}

	void ShaderBase::destroy() {
		LOG_TRACE("ShaderBase") << "Shader " << m_programID << " deleted. " << Console::endl;
		if (m_compiled != 0) {
			glDeleteProgram(m_programID);
			m_programID = 0;
		}
	}

	void ShaderBase::use() const {
		//LOG_TRACE("Shader") << "Using program : " << m_programID << Console::endl;
		if (!isCompiled()) {
			//LOG_ERROR("Shader") << "Failed to bind shader. Program is not compiled" << Console::endl;
			return;
		}
		glUseProgram(m_programID);
	}


	void ShaderBase::attach(GenericBufferObject& buf) {
		int block_index = glGetProgramResourceIndex(m_programID, GL_SHADER_STORAGE_BLOCK, buf.name().c_str());
		if (block_index == -1) Console::error("ShaderBase") << "Block " << buf.name() << " not found in shader '" << m_name << "'. Did you bind it properly ?" << Console::endl;
		else {
			GLuint bindingPoint = buf.bindingPoint();
			buf.bind();
			Console::info("ShaderBase") << buf.name() << "( block index " << block_index << ") is now bound to " << name() << " using binding point " << bindingPoint << Console::endl;
			glShaderStorageBlockBinding(m_programID, block_index, bindingPoint);//Do this explicitly in your shader !
		}
	}

	//TODO : bind the shaders automatically before setting uniforms.
	GLuint ShaderBase::getUniformLocation(const char* uniform) const{
		if (!isCompiled()) return 0;
		GLuint uniLoc = glGetUniformLocation(m_programID, uniform);
		if (uniLoc == -1);// LOG_WARN("Shader") << "(" << m_name << ") Invalid Uniform name : " << uniform << ", (or wrong binded shader)" << Console::endl;
		return uniLoc;
	}

	void ShaderBase::setUInt(const std::string name, GLuint value) const {
		if (!isCompiled()) return;
		int uniformLocation = getUniformLocation(name.c_str());
		glUniform1ui(uniformLocation, value);
	}

	void ShaderBase::setInt(const std::string name, GLint value) const {
		if (!isCompiled()) return;
		int uniformLocation = getUniformLocation(name.c_str());
		glUniform1i(uniformLocation, value);
	}

	void ShaderBase::setFloat(const std::string name, GLfloat value) const {
		if (!isCompiled()) return;
		int uniformLocation = getUniformLocation(name.c_str());
		glUniform1f(uniformLocation, value);
	}

	void ShaderBase::setDouble(const std::string name, GLdouble value) const {
		if (!isCompiled()) return;
		int uniformLocation = getUniformLocation(name.c_str());
		glUniform1d(uniformLocation, value);
	}

	void ShaderBase::setMat4(const std::string name, glm::mat4 mat) const {
		if (!isCompiled()) return;
		glUniformMatrix4fv(getUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
	}

	void ShaderBase::setVec4(const std::string name, glm::vec4 value) const {
		if (!isCompiled()) return;
		glUniform4fv(getUniformLocation(name.c_str()), 1, glm::value_ptr(value));
	}

	void ShaderBase::setVec3(const std::string name, glm::vec3 value) const {
		if (!isCompiled()) return;
		glUniform3fv(getUniformLocation(name.c_str()), 1, glm::value_ptr(value));
	}

	void ShaderBase::setVec2(const std::string name, glm::vec2 value) const {
		if (!isCompiled()) return;
		glUniform2fv(getUniformLocation(name.c_str()), 1, glm::value_ptr(value));
	}

	void ShaderBase::setIntArray(const std::string name, GLint* values, uint32_t count) const {
		if (!isCompiled()) return;
		glUniform2iv( getUniformLocation(name.c_str()), count, values);
	}

	std::string ShaderBase::readSrc(const std::string& filename)
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

				std::string includeContent = readSrc(absPath); // Recursive call to load included file's content
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
	std::string ShaderBase::readSrc(const std::string filename)
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
	std::shared_ptr<ShaderBase> ShaderBase::create(const std::string& name) {
		return std::make_shared<ShaderBase>(name);
	}
	*/


}

