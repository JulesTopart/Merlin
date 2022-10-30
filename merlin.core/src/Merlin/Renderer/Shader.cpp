#include "glpch.h"
#include "Shader.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>


namespace Merlin::Renderer {
	Shader::Shader(std::string n) : ShaderBase(n){}
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

	void Shader::Compile(	const std::string vertex_file_path,
							const std::string fragment_file_path,
							const std::string geometry_file_path) {
		_compiled = true;

		// Read vertexFile and fragmentFile and store the strings
		LOG_INFO() << "Importing shader source... : " << vertex_file_path << Console::endl;
		VertexShaderSrc = ReadSrc(vertex_file_path);
		 
		LOG_INFO() << "Importing shader source... : " << fragment_file_path << Console::endl;
		FragmentShaderSrc = ReadSrc(fragment_file_path);

		LOG_INFO() << "Creating shaders... : " << Console::endl;
		vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);


		GLint Result = GL_FALSE;
		int InfoLogLength;

		// Compile Vertex Shader
		//LOG << ("Compiling vertex shader...") << Console::endl;
		const char* VertexSrcPointer = VertexShaderSrc.c_str();
		glShaderSource(vertexShaderID, 1, &VertexSrcPointer, NULL);
		glCompileShader(vertexShaderID);

		// Check Vertex Shader
		glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> VertexShaderErrorMessage(int(InfoLogLength) + 1);
			glGetShaderInfoLog(vertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			LOG_ERROR("Shader") << &VertexShaderErrorMessage[0] << Console::endl;
			LOG_ERROR("Shader") << "Vertex shader compilation failed." << Console::endl;
			_compiled = false;
		}
		else LOG_OK("Shader") << "Vertex shader compilation sucessful." << Console::endl;

		// Compile Fragment Shader
		//LOG << "Compiling fragment shader..." << Console::endl;
		const char* FragmentSrcPointer = FragmentShaderSrc.c_str();
		glShaderSource(fragmentShaderID, 1, &FragmentSrcPointer, NULL);
		glCompileShader(fragmentShaderID);

		// Check Fragment Shader
		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> FragmentShaderErrorMessage(int(InfoLogLength) + 1);
			glGetShaderInfoLog(fragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			LOG_ERROR("Shader") << &FragmentShaderErrorMessage[0];
			LOG_ERROR("Shader") << "Fragment shader compilation failed." << Console::endl;
			_compiled = false;
		}
		else LOG_OK("Shader") << "Fragment shader compilation sucessful." << Console::endl;

		SetID(glCreateProgram());
		

		LOG_INFO("Shader") << "Creating program n " << id() << "..." << Console::endl;

		glAttachShader(id(), vertexShaderID);
		glAttachShader(id(), fragmentShaderID);
		glLinkProgram(id());

		// Check the program
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

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
	}
}
