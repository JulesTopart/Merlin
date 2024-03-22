#include "glpch.h"
#include "Shader.h"

#include "Merlin/Util/Util.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include "Merlin/Core/Log.h"

namespace Merlin {

	Shared<Shader> Shader::create(	const std::string& name, 
									const std::string& vertex_file_path,
									const std::string& fragment_file_path,
									const std::string& geometry_file_path) {

		return createShared<Shader>(name, vertex_file_path, fragment_file_path, geometry_file_path);
	}

	Shader::Shader() : ShaderBase(ShaderType::GRAPHICS) {}
	Shader::Shader(std::string n) : ShaderBase(n, ShaderType::GRAPHICS) {}

	Shader::Shader(std::string n,
		const std::string vpath,
		const std::string fpath,
		const std::string gpath) : ShaderBase(n, ShaderType::GRAPHICS) {

		if (vpath != "" && fpath != "") {
			compile(vpath, fpath, gpath);
			if (isCompiled()) use();
		}
	}

	Shader::~Shader() {
		destroy();
	}

	void Shader::destroy() {
		LOG_TRACE("Shader") << "Destructing Shader " << id() << " deleted. " << Console::endl;
		if (m_compiled != 0) {

			vertexShaderID = 0;
			fragmentShaderID = 0;
			geometryShaderID = 0;
		}
	}


	void Shader::compileShader(const std::string& name, const std::string& src, GLuint id) {

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
			m_compiled = false;
		}
		else LOG_OK("Shader") << name << " shader compilation sucessful." << Console::endl;
	}



	
	void Shader::compile(const std::string& vertex_file_path,
		const std::string& fragment_file_path,
		const std::string& geometry_file_path) {
		m_compiled = true;

		VertexShaderSrc = "";;
		FragmentShaderSrc = "";
		GeomShaderSrc = "";

		// Read vertexFile and fragmentFile and store the strings
		LOG_INFO() << "Importing shader source... : " << vertex_file_path << Console::endl;
		VertexShaderSrc = readSrc(vertex_file_path);

		LOG_INFO() << "Importing shader source... : " << fragment_file_path << Console::endl;
		FragmentShaderSrc = readSrc(fragment_file_path);

		if (geometry_file_path != "") {
			LOG_INFO() << "Importing shader source... : " << geometry_file_path << Console::endl;
			GeomShaderSrc = readSrc(geometry_file_path);
		}

		compileFromSrc(VertexShaderSrc, FragmentShaderSrc, GeomShaderSrc);

	}


	void Shader::compileFromSrc(const std::string& VertexShaderSrc,
								const std::string& FragmentShaderSrc,
								const std::string& GeomShaderSrc) {
		m_compiled = true;

		LOG_TRACE() << "Creating shaders... : " << Console::endl;
		vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		if (GeomShaderSrc != "")
			geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);


		// Compile Vertex Shader
		compileShader("Vertex", VertexShaderSrc, vertexShaderID);
		compileShader("Fragment", FragmentShaderSrc, fragmentShaderID);
		if (GeomShaderSrc != "")
			compileShader("Vertex", GeomShaderSrc, geometryShaderID);

		setID(glCreateProgram());

		LOG_INFO("Shader") << "Creating program n " << id() << "..." << Console::endl;

		glAttachShader(id(), vertexShaderID);
		glAttachShader(id(), fragmentShaderID);
		if (GeomShaderSrc != "")
			glAttachShader(id(), geometryShaderID);

		glLinkProgram(id());

		// Check the program
		GLint Result = GL_FALSE;
		int InfoLogLength;

		glGetProgramiv(id(), GL_LINK_STATUS, &Result);
		glGetProgramiv(id(), GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(int(InfoLogLength) + 1);
			glGetProgramInfoLog(id(), InfoLogLength, NULL, &ProgramErrorMessage[0]);
			LOG_ERROR() << &ProgramErrorMessage[0];
			LOG_ERROR("Shader") << "Shader : " << name() << " program linkage failed." << Console::endl;
			m_compiled = false;
		}

		if (Result == GL_FALSE) Console::error("Shader") << "Shader " << id() << " not linked" << Console::endl;
		else LOG_OK("Shader") << "Shader program : " << name() << "  successfully created." << Console::endl;


		glDetachShader(id(), vertexShaderID);
		glDetachShader(id(), fragmentShaderID);

		if (GeomShaderSrc != "")
			glDetachShader(id(), geometryShaderID);

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
		if (GeomShaderSrc != "")
			glDeleteShader(geometryShaderID);

	}

}
