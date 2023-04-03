#include "glpch.h"
#include "Shader.h"

#include "Merlin/Util/Util.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include "Merlin/Core/Log.h"

namespace Merlin::Graphics {

	Shared<Shader> Shader::Create(const std::string& vertex_file_path,
		const std::string& fragment_file_path,
		const std::string& geometry_file_path) {
		return CreateShared<Shader>(Utils::GetFileName(vertex_file_path), vertex_file_path, fragment_file_path, geometry_file_path);
	}

	Shader::Shader(std::string n) : ShaderBase(n) {}

	Shader::Shader(std::string n,
		const std::string vpath,
		const std::string fpath,
		const std::string gpath) : ShaderBase(n) {

		if (vpath != "" && fpath != "") {
			Compile(vpath, fpath, gpath);
		}
	}

	Shader::~Shader() {
		Delete();
	}

	void Shader::Delete() {
		LOG_TRACE("Shader") << "Destructing Shader " << id() << " deleted. " << Console::endl;
		if (_compiled != 0) {

			vertexShaderID = 0;
			fragmentShaderID = 0;
			geometryShaderID = 0;
		}
	}


	void Shader::CompileShader(const std::string& name, const std::string& src, GLuint id) {

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
			_compiled = false;
		}
		else LOG_OK("Shader") << name << " shader compilation sucessful." << Console::endl;
	}



	
	void Shader::Compile(const std::string& vertex_file_path,
		const std::string& fragment_file_path,
		const std::string& geometry_file_path) {
		_compiled = true;

		// Read vertexFile and fragmentFile and store the strings
		LOG_INFO() << "Importing shader source... : " << vertex_file_path << Console::endl;
		VertexShaderSrc = ReadSrc(vertex_file_path);

		LOG_INFO() << "Importing shader source... : " << fragment_file_path << Console::endl;
		FragmentShaderSrc = ReadSrc(fragment_file_path);

		if (geometry_file_path != "") {
			LOG_INFO() << "Importing shader source... : " << geometry_file_path << Console::endl;
			GeomShaderSrc = ReadSrc(geometry_file_path);
		}

		LOG_TRACE() << "Creating shaders... : " << Console::endl;
		vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		if (geometry_file_path != "")
			geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);


		// Compile Vertex Shader
		CompileShader("Vertex", VertexShaderSrc, vertexShaderID);
		CompileShader("Fragment", FragmentShaderSrc, fragmentShaderID);
		if (geometry_file_path != "")
			CompileShader("Vertex", GeomShaderSrc, geometryShaderID);

		SetID(glCreateProgram());

		LOG_INFO("Shader") << "Creating program n " << id() << "..." << Console::endl;

		glAttachShader(id(), vertexShaderID);
		glAttachShader(id(), fragmentShaderID);
		if (geometry_file_path != "")
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
			LOG_ERROR("Shader") << "Shader program failed." << Console::endl;
			_compiled = false;
		}

		if (Result == GL_FALSE) Console::error("Shader") << "Shader " << id() << " not linked" << Console::endl;
		else LOG_OK("Shader") << "Shader program successfully created." << Console::endl;


		glDetachShader(id(), vertexShaderID);
		glDetachShader(id(), fragmentShaderID);

		if (geometry_file_path != "")
			glDetachShader(id(), geometryShaderID);

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
		if (geometry_file_path != "")
			glDeleteShader(geometryShaderID);

	}
	

	

	void Shader::CompileFromSrc(const std::string& VertexShaderSrc,
								const std::string& FragmentShaderSrc,
								const std::string& GeomShaderSrc) {
		_compiled = true;

		LOG_TRACE() << "Creating shaders... : " << Console::endl;
		vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		if (GeomShaderSrc != "")
			geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);


		// Compile Vertex Shader
		CompileShader("Vertex", VertexShaderSrc, vertexShaderID);
		CompileShader("Fragment", FragmentShaderSrc, vertexShaderID);
		if (GeomShaderSrc != "")
			CompileShader("Vertex", GeomShaderSrc, vertexShaderID);



		SetID(glCreateProgram());

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
			LOG_ERROR("Shader") << "Shader program failed." << Console::endl;
			_compiled = false;
		}
		else LOG_OK("Shader") << "Shader program successfully created." << Console::endl;

		glDetachShader(id(), vertexShaderID);
		glDetachShader(id(), fragmentShaderID);

		if (GeomShaderSrc != "")
			glDetachShader(id(), geometryShaderID);

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
		if (GeomShaderSrc != "")
			glDeleteShader(geometryShaderID);

	}



	std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderLibrary::m_Shaders;

	void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader)
	{
		GLCORE_ASSERT(!Exists(name), "ShaderLibrary", "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader)
	{
		auto& name = shader->name();
		Add(name, shader);
	}

	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name)
	{
		auto shader = CreateShared<Shader>(name);
		Add(shader);
		return shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name)
	{
		GLCORE_ASSERT(Exists(name), "ShaderLibrary", "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name)
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}

}
