#include "glpch.h"
#include "shaderBase.h"
#include "merlin/core/log.h"
#include "merlin/memory/bindingPointManager.h"
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

	void ShaderBase::attach(AbstractBufferObject& buf) {
		int block_index = glGetProgramResourceIndex(m_programID, GL_SHADER_STORAGE_BLOCK, buf.name().c_str());
		if (block_index == -1) Console::error("ShaderBase") << "Block " << buf.name() << " not found in shader '" << m_name << "'. Did you bind it properly ?" << Console::endl;
		else {
			BindingPointManager& manager = BindingPointManager::instance();
			auto bindingPoint = manager.allocateBindingPoint(buf.target(), buf.id());
			buf.bind();
			buf.setBindingPoint(bindingPoint);
			Console::trace("ShaderBase") << buf.name() << "( block index " << block_index << ") is now bound to " << name() << " using binding point " << bindingPoint << Console::endl;
			glShaderStorageBlockBinding(m_programID, block_index, bindingPoint);//Do this explicitly in your shader !
			buf.unbind();
		}
	}

	void ShaderBase::detach(AbstractBufferObject& buf){
		buf.releaseBindingPoint();
	}

	//TODO : bind the shaders automatically before setting uniforms.
	GLuint ShaderBase::getUniformLocation(const char* uniform) const{
		if (!isCompiled()) return 0;
		GLuint uniLoc = glGetUniformLocation(m_programID, uniform);
		if (uniLoc == -1) LOG_WARN("Shader") << "(" << m_name << ") Invalid Uniform name : " << uniform << ", (or wrong binded shader)" << Console::endl;
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

	void ShaderBase::setMat3(const std::string name, glm::mat3 mat) const {
		if (!isCompiled()) return;
		glUniformMatrix3fv(getUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
	}

	void ShaderBase::setVec4(const std::string name, glm::vec4 value) const {
		if (!isCompiled()) return;
		glUniform4fv(getUniformLocation(name.c_str()), 1, glm::value_ptr(value));
	}

	void ShaderBase::setIVec4(const std::string name, glm::ivec4 value) const {
		if (!isCompiled()) return;
		glUniform4i(getUniformLocation(name.c_str()), value.x, value.y, value.z, value.w);
	}

	void ShaderBase::setUVec4(const std::string name, glm::uvec4 value) const {
		if (!isCompiled()) return;
		glUniform4ui(getUniformLocation(name.c_str()), value.x, value.y, value.z, value.w);
	}

	void ShaderBase::setDVec4(const std::string name, glm::dvec4 value) const {
		if (!isCompiled()) return;
		glUniform4d(getUniformLocation(name.c_str()), value.x, value.y, value.z, value.w);
	}

	void ShaderBase::setVec3(const std::string name, glm::vec3 value) const {
		if (!isCompiled()) return;
		glUniform3fv(getUniformLocation(name.c_str()), 1, glm::value_ptr(value));
	}

	void ShaderBase::setIVec3(const std::string name, glm::ivec3 value) const {
		if (!isCompiled()) return;
		glUniform3i(getUniformLocation(name.c_str()), value.x, value.y, value.z);
	}

	void ShaderBase::setUVec3(const std::string name, glm::uvec3 value) const {
		if (!isCompiled()) return;
		glUniform3ui(getUniformLocation(name.c_str()), value.x, value.y, value.z);
	}

	void ShaderBase::setDVec3(const std::string name, glm::dvec3 value) const {
		if (!isCompiled()) return;
		glUniform3d(getUniformLocation(name.c_str()), value.x, value.y, value.z);
	}

	void ShaderBase::setVec2(const std::string name, glm::vec2 value) const {
		if (!isCompiled()) return;
		glUniform2fv(getUniformLocation(name.c_str()), 1, glm::value_ptr(value));
	}

	void ShaderBase::setIVec2(const std::string name, glm::ivec2 value) const {
		if (!isCompiled()) return;
		glUniform2i(getUniformLocation(name.c_str()), value.x, value.y);
	}

	void ShaderBase::setUVec2(const std::string name, glm::uvec2 value) const {
		if (!isCompiled()) return;
		glUniform2ui(getUniformLocation(name.c_str()), value.x, value.y);
	}

	void ShaderBase::setDVec2(const std::string name, glm::dvec2 value) const {
		if (!isCompiled()) return;
		glUniform2d(getUniformLocation(name.c_str()), value.x, value.y);
	}

	void ShaderBase::setIntArray(const std::string name, GLint* values, uint32_t count) const {
		if (!isCompiled()) return;
		glUniform2iv( getUniformLocation(name.c_str()), count, values);
	}

	// -----------

	bool ShaderBase::hasConstant(const std::string& name) const{
		return m_constants.find(name) != m_constants.end();
	}

	void ShaderBase::setConstUInt(const std::string name, GLuint value) {
		m_constants[name] = "const uint " + name + " = " + std::to_string(value);
	}

	void ShaderBase::setConstInt(const std::string name, GLint value) {
		m_constants[name] = "const int " + name + " = " + std::to_string(value);
	}

	void ShaderBase::setConstFloat(const std::string name, GLfloat value) {
		m_constants[name] = "const float " + name + " = " + std::to_string(value);
	}

	void ShaderBase::setConstDouble(const std::string name, GLdouble value) {
		m_constants[name] = "const double " + name + " = " + std::to_string(value);
	}

	void ShaderBase::setConstMat4(const std::string name, glm::mat4 mat) {
		std::string value = "";
		
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				value += std::to_string(float(mat[i][j])) + ((i + j * 4 < 15) ? "," : "");

		m_constants[name] = "const mat4 " + name + " = " + value;
	}

	void ShaderBase::setConstMat3(const std::string name, glm::mat3 mat) {
		std::string value = "";

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				value += std::to_string(float(mat[i][j])) + ((i + j * 4 < 15) ? "," : "");

		m_constants[name] = "const mat3 " + name + " = " + value;
	}

	void ShaderBase::setConstDMat3(const std::string name, glm::dmat3 mat) {
		std::string value = "";

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				value += std::to_string(double(mat[i][j])) + ((i + j * 4 < 15) ? "," : "");

		m_constants[name] = "const dmat3 " + name + " = " + value;
	}

	void ShaderBase::setConstDMat4(const std::string name, glm::dmat4 mat) {
		std::string value = "";

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				value += std::to_string(double(mat[i][j])) + ((i + j * 4 < 15) ? "," : "");

		m_constants[name] = "const dmat4 " + name + " = " + value;
	}



	void ShaderBase::setConstVec4(const std::string name, glm::vec4 vec) {
		std::string value = "";

		for (int i = 0; i < 4; i++)
				value += std::to_string(float(vec[i])) + ((i < 3) ? "," : "");

		m_constants[name] = "const vec4 " + name + " = vec4(" + value + ")";
	}

	void ShaderBase::setConstIVec4(const std::string name, glm::ivec4 vec) {
		std::string value = "";

		for (int i = 0; i < 4; i++)
			value += std::to_string(int(vec[i])) + ((i < 3) ? "," : "");

		m_constants[name] = "const ivec4 " + name + " = ivec4(" + value + ")";
	}

	void ShaderBase::setConstUVec4(const std::string name, glm::uvec4 vec) {
		std::string value = "";

		for (int i = 0; i < 4; i++)
			value += std::to_string(GLuint(vec[i])) + ((i < 3) ? "," : "");

		m_constants[name] = "const uvec4 " + name + " = uvec4(" + value + ")";
	}

	void ShaderBase::setConstDVec4(const std::string name, glm::dvec4 vec) {
		std::string value = "";

		for (int i = 0; i < 4; i++)
			value += std::to_string(GLuint(vec[i])) + ((i < 3) ? "," : "");

		m_constants[name] = "const dvec4 " + name + " = dvec4(" + value + ")";
	}

	void ShaderBase::setConstVec3(const std::string name, glm::vec3 vec) {
		std::string value = "";

		for (int i = 0; i < 3; i++)
			value += std::to_string(float(vec[i])) + ((i < 2) ? "," : "");

		m_constants[name] = "const vec3 " + name + " = vec3(" + value + ")";
	}

	void ShaderBase::setConstIVec3(const std::string name, glm::ivec3 vec) {
		std::string value = "";

		for (int i = 0; i < 3; i++)
			value += std::to_string(int(vec[i])) + ((i < 2) ? "," : "");

		m_constants[name] = "const ivec3 " + name + " = ivec3(" + value + ")";
	}

	void ShaderBase::setConstUVec3(const std::string name, glm::uvec3 vec) {
		std::string value = "";

		for (int i = 0; i < 3; i++)
			value += std::to_string(GLuint(vec[i])) + ((i < 2) ? "," : "");

		m_constants[name] = "const uvec3 " + name + " = uvec3(" + value + ")";
	}

	void ShaderBase::setConstDVec3(const std::string name, glm::dvec3 vec) {
		std::string value = "";

		for (int i = 0; i < 3; i++)
			value += std::to_string(GLuint(vec[i])) + ((i < 2) ? "," : "");

		m_constants[name] = "const dvec3 " + name + " = dvec3(" + value + ")";
	}

	void ShaderBase::setConstVec2(const std::string name, glm::vec2 vec)  {
		std::string value = "";

		for (int i = 0; i < 2; i++)
			value += std::to_string(float(vec[i])) + ((i < 1) ? "," : "");

		m_constants[name] = "const vec2 " + name + " = vec2(" + value + ")";
	}

	void ShaderBase::setConstIVec2(const std::string name, glm::ivec2 vec) {
		std::string value = "";

		for (int i = 0; i < 2; i++)
			value += std::to_string(float(vec[i])) + ((i < 1) ? "," : "");

		m_constants[name] = "const ivec2 " + name + " = ivec2(" + value + ")";
	}

	void ShaderBase::setConstUVec2(const std::string name, glm::uvec2 vec) {
		std::string value = "";

		for (int i = 0; i < 2; i++)
			value += std::to_string(GLuint(vec[i])) + ((i < 1) ? "," : "");

		m_constants[name] = "const uvec2 " + name + " = uvec2(" + value + ")";
	}

	void ShaderBase::setConstDVec2(const std::string name, glm::dvec2 vec) {
		std::string value = "";

		for (int i = 0; i < 2; i++)
			value += std::to_string(GLuint(vec[i])) + ((i < 1) ? "," : "");

		m_constants[name] = "const dvec2 " + name + " = dvec2(" + value + ")";
	}

	void ShaderBase::define(const std::string& name, const std::string& value) {
		m_defines[name] = value;
	}




	// -----------

// Function to remove single-line comments (//...)
	std::string removeSingleLineComments(const std::string& content) {
		std::regex singleLineCommentRegex(R"(//.*)");
		return std::regex_replace(content, singleLineCommentRegex, "");
	}

	// Function to remove multi-line comments (/*...*/)
	std::string removeMultiLineComments(const std::string& content) {
		std::string result = content;
		size_t start_pos = 0;
		while ((start_pos = result.find("/*", start_pos)) != std::string::npos) {
			size_t end_pos = result.find("*/", start_pos + 2);
			if (end_pos != std::string::npos) {
				end_pos += 2; // Move past the end of the comment
				result.erase(start_pos, end_pos - start_pos);
			}
			else {
				// If there is no closing */, remove till end of the string
				result.erase(start_pos);
				break;
			}
		}
		return result;
	}

	// Function to read and process the shader file content
	std::string ShaderBase::readSrc(const std::string& filename) {
		std::ifstream in(filename, std::ios::binary);
		if (in) {
			std::string content;
			in.seekg(0, std::ios::end);
			content.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&content[0], content.size());
			in.close();

			// Find and replace include statements
			std::regex includeRegex(R"(#include\s+\"([^"]+)\")");
			std::smatch includeMatch;

			std::string path = filename.substr(0, filename.find_last_of('/'));

			content = removeSingleLineComments(content);
			content = removeMultiLineComments(content);

			while (std::regex_search(content, includeMatch, includeRegex)) {
				std::string includeFile = includeMatch[1].str();
				std::string absPath = path + "/" + includeFile;

				std::string includeContent = readSrc(absPath); // Recursive call to load included file's content
				content = std::regex_replace(content, includeRegex, includeContent, std::regex_constants::format_first_only);
			}

			return content;
		}
		else {
			std::cerr << "Can't read file " << filename << std::endl;
			return "error";
		}
	}


	void ShaderBase::precompileSrc(std::string& src){
		src = removeSingleLineComments(src);
		src = removeMultiLineComments(src);
		src = updateConstants(src);
		src = updateDefines(src);

		;
	}

	bool ShaderBase::compileShader(const std::string& name,const std::string& src, GLuint id) {
		bool result = true;
		GLint Result = GL_FALSE;
		int InfoLogLength;

		// Compile Shader
		const char* SrcPointer = src.c_str();
		glShaderSource(id, 1, &SrcPointer, NULL);
		glCompileShader(id);

		// Check Shader
		glGetShaderiv(id, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ShaderErrorMessage(int(InfoLogLength) + 1);
			glGetShaderInfoLog(id, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			LOG_ERROR("Shader") << &ShaderErrorMessage[0] << Console::endl;
			LOG_ERROR("Shader") << name << " shader compilation failed." << Console::endl;
			result = false;
		}
		else LOG_OK("Shader") << name << " shader compilation sucessful." << Console::endl;

		return result;
	}

	std::string ShaderBase::updateConstants(const std::string& originalSrc){
		std::string src = originalSrc;
		for (const auto& cst : m_constants) {
			size_t cst_pos = src.find(cst.first);

			if (cst_pos != std::string::npos) { // If the constant exists, replace it
				size_t line_start = src.rfind('\n', cst_pos);
				if (line_start == std::string::npos) {
					line_start = 0; // Start of the file
				}
				else {
					line_start++; // Move to the start of the line
				}
				size_t line_end = src.find(';', cst_pos);
				if (line_end != std::string::npos) {
					line_end++; // Move past the semicolon
					src.replace(line_start, line_end - line_start, cst.second + ";");
				}
			}
		}
		return src;
	}

	std::string ShaderBase::updateDefines(const std::string& originalSrc){
		std::string src = originalSrc;
		for (const auto& cst : m_defines) {
			size_t cst_pos = src.find(cst.first);

			if (cst_pos != std::string::npos) { // If the constant exists, replace it
				size_t line_start = src.rfind('\n', cst_pos);
				if (line_start == std::string::npos) {
					line_start = 0; // Start of the file
				}
				else {
					line_start++; // Move to the start of the line
				}
				size_t line_end = src.find('\n', cst_pos);
				if (line_end != std::string::npos) {
					src.replace(line_start, line_end - line_start, "#define " + cst.first + " " + cst.second);
				}
				else {
					size_t pos = src.find("#version");
					if (pos != std::string::npos) {
						pos = src.find("\n", pos); // Move to the end of the #version line
						if (pos != std::string::npos) {
							++pos; // Move to the start of the next line
							src.insert(pos, cst.second + "\n");
						}
					}
				}
			}
		}
		return src;
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

