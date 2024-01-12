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

	Shared<Shader> Shader::Create(	const std::string& name, 
									const std::string& vertex_file_path,
									const std::string& fragment_file_path,
									const std::string& geometry_file_path) {

		return CreateShared<Shader>(name, vertex_file_path, fragment_file_path, geometry_file_path);
	}

	Shader::Shader(std::string n) : ShaderBase(n) {}

	Shader::Shader(std::string n,
		const std::string vpath,
		const std::string fpath,
		const std::string gpath) : ShaderBase(n) {

		if (vpath != "" && fpath != "") {
			Compile(vpath, fpath, gpath);
			if (IsCompiled()) Use();
		}
	}

	Shader::~Shader() {
		Delete();
	}

	void Shader::Delete() {
		LOG_TRACE("Shader") << "Destructing Shader " << id() << " deleted. " << Console::endl;
		if (m_compiled != 0) {

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
			m_compiled = false;
		}
		else LOG_OK("Shader") << name << " shader compilation sucessful." << Console::endl;
	}



	
	void Shader::Compile(const std::string& vertex_file_path,
		const std::string& fragment_file_path,
		const std::string& geometry_file_path) {
		m_compiled = true;

		VertexShaderSrc = "";;
		FragmentShaderSrc = "";
		GeomShaderSrc = "";

		// Read vertexFile and fragmentFile and store the strings
		LOG_INFO() << "Importing shader source... : " << vertex_file_path << Console::endl;
		VertexShaderSrc = ReadSrc(vertex_file_path);

		LOG_INFO() << "Importing shader source... : " << fragment_file_path << Console::endl;
		FragmentShaderSrc = ReadSrc(fragment_file_path);

		if (geometry_file_path != "") {
			LOG_INFO() << "Importing shader source... : " << geometry_file_path << Console::endl;
			GeomShaderSrc = ReadSrc(geometry_file_path);
		}

		CompileFromSrc(VertexShaderSrc, FragmentShaderSrc, GeomShaderSrc);

	}


	void Shader::CompileFromSrc(const std::string& VertexShaderSrc,
								const std::string& FragmentShaderSrc,
								const std::string& GeomShaderSrc) {
		m_compiled = true;

		LOG_TRACE() << "Creating shaders... : " << Console::endl;
		vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		if (GeomShaderSrc != "")
			geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);


		// Compile Vertex Shader
		CompileShader("Vertex", VertexShaderSrc, vertexShaderID);
		CompileShader("Fragment", FragmentShaderSrc, fragmentShaderID);
		if (GeomShaderSrc != "")
			CompileShader("Vertex", GeomShaderSrc, geometryShaderID);

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

	ShaderLibrary::ShaderLibrary(){

		Shared<Shader> defaultShader = CreateShared<Shader>("default");

		std::string vertexSrc =
			"#version 330 core\n"
			"layout(location = 0) in vec3 _position;\n"
			"layout(location = 1) in vec3 _normal;\n"
			"layout(location = 2) in vec3 _color;\n"
			"layout(location = 3) in vec2 _texCoord;\n"
			"out vec3 position;\n"
			"out vec3 normal;\n"
			"out vec3 color;\n"
			"out vec2 texCoord;\n"
			"uniform vec3 viewPos;\n"
			"uniform mat4 view;\n"
			"uniform mat4 model;\n"
			"uniform mat4 projection;\n"
			"void main() {\n"
			"position = vec3(model * vec4(_position, 1.0f));\n"
			"color = _color;\n"
			"normal = _normal;\n"
			"texCoord = _texCoord;\n"
			"gl_Position = projection * view * vec4(position, 1.0f);\n"
			"}";

		std::string fragmentSrc =
			"#version 330 core\n"
			"in vec3 position;\n"
			"in vec3 normal;\n"
			"in vec3 color;\n"
			"in vec2 texCoord;\n"
			"uniform vec3 viewPos;\n"
			"uniform vec3 ambient;\n"
			"uniform vec3 diffuse;\n"
			"uniform vec3 specular;\n"
			"uniform float shininess;\n"
			"out vec4 FragColor;\n"
			"void main() {\n"
			"vec3 lightPos = vec3(100);\n"
			"vec3 n = normalize(normal);\n"
			"vec3 lightDir = normalize(lightPos - position);\n"
			"vec3 viewDir = normalize(viewPos - position);\n"
			"vec3 reflectDir = reflect(-lightDir, n);\n"
			"vec3 _ambient = ambient *color;\n"
			"vec3 _diffuse = diffuse * max(dot(n, lightDir), 0.0f);\n"
			"vec3 _specular = specular * pow(max(dot(viewDir, reflectDir), 0.0f), max(shininess, 8));\n"
			"FragColor = vec4(_ambient + _diffuse + _specular, 1.0f);\n"
			"}";

		defaultShader->CompileFromSrc(vertexSrc, fragmentSrc);
		defaultShader->noTexture();
		//defaultShader->noMaterial();
		Add(defaultShader);

		/*
		Shared<Shader> debugShader = Shader::Create("debug.normals", "assets/common/shaders/debug.normals.vert", "assets/common/shaders/debug.normals.frag", "assets/common/shaders/debug.normals.geom");
		debugShader->noTexture();
		debugShader->noMaterial();
		Add(debugShader);*/
	}

	void ShaderLibrary::Add(Shared<Shader> shader){
		if(Exists(shader->name())) Console::warn("ShaderLibrary") << "Shader already exists ! " << shader->name() << " shader has been overridden." << Console::endl;
		_shaders[shader->name()] = shader;
	}

	void ShaderLibrary::Load(const std::string& name){
		auto shader = CreateShared<Shader>(name);
		Add(shader);
	}

	const Shader& ShaderLibrary::Get(const std::string & name) {
		if (!Exists(name)) {
			Console::error("ShaderLibrary") << "Shader " << name << " not found ! Using default shader instead." << Console::endl;
			return Get("default");
		}
		return *_shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name){
		return _shaders.find(name) != _shaders.end();
	}

	Shared<Shader> ShaderLibrary::Share(const std::string& name) {
		if (!Exists(name)) {
			Console::error("ShaderLibrary") << "Shader " << name << " not found ! Using default shader instead." << Console::endl;
			return Share("default");
		}
		return _shaders[name];
	}

}
