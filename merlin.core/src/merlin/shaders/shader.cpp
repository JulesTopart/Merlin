#include "glpch.h"
#include "shader.h"

#include "merlin/utils/util.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include "merlin/core/log.h"

namespace Merlin {

	Shared<Shader> Shader::create(	const std::string& name, 
									const std::string& vertex_file_path,
									const std::string& fragment_file_path,
									const std::string& geometry_file_path, bool compile) {

		return createShared<Shader>(name, vertex_file_path, fragment_file_path, geometry_file_path, compile);
	}

	Shader::Shader() : ShaderBase(ShaderType::GRAPHICS) {}
	Shader::Shader(std::string n) : ShaderBase(n, ShaderType::GRAPHICS) {}


	Shader::Shader(std::string n,
		const std::string vpath,
		const std::string fpath,
		const std::string gpath, bool compile) : ShaderBase(n, ShaderType::GRAPHICS) {

		if (vpath != "" && fpath != "")
		if (compile) compileFromFile(vpath, fpath, gpath);
		else readFile(vpath, fpath, gpath);
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
	
	void Shader::compile() {
		compileFromSrc(VertexShaderSrc, FragmentShaderSrc, GeomShaderSrc);
	}
	
	void Shader::compileFromFile(const std::string& vertex_file_path,
		const std::string& fragment_file_path,
		const std::string& geometry_file_path) {

		readFile(vertex_file_path, fragment_file_path, geometry_file_path);
		compile();
	}

	void Shader::readFile(const std::string& vertex_file_path,
		const std::string& fragment_file_path,
		const std::string& geometry_file_path) {

		VertexShaderSrc = "";
		FragmentShaderSrc = "";
		GeomShaderSrc = "";

		// Read vertexFile and fragmentFile and store the strings
		LOG_INFO() << "Importing Vertex shader source... : " << vertex_file_path << Console::endl;
		VertexShaderSrc = readSrc(vertex_file_path);

		LOG_INFO() << "Importing Fragment shader source... : " << fragment_file_path << Console::endl;
		FragmentShaderSrc = readSrc(fragment_file_path);

		if (geometry_file_path != "") {
			LOG_INFO() << "Importing Geometry shader source... : " << geometry_file_path << Console::endl;
			GeomShaderSrc = readSrc(geometry_file_path);
		}
	}


	void Shader::compileFromSrc(const std::string& vSrc,
								const std::string& fSrc,
								const std::string& gSrc) {
		m_compiled = true;

		LOG_TRACE() << "Creating shaders... : " << Console::endl;
		vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		if (GeomShaderSrc != "")
			geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);

		VertexShaderSrc = vSrc;
		FragmentShaderSrc = fSrc;
		GeomShaderSrc = gSrc;

		precompileSrc(VertexShaderSrc);
		precompileSrc(FragmentShaderSrc);
		precompileSrc(GeomShaderSrc);

		// Compile Vertex Shader
		compileShader("Vertex", VertexShaderSrc, vertexShaderID);
		compileShader("Fragment", FragmentShaderSrc, fragmentShaderID);
		if (GeomShaderSrc != "")
			compileShader("Geometry", GeomShaderSrc, geometryShaderID);

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

		use();
	}

}
