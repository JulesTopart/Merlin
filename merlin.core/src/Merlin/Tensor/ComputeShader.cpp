#include "glpch.h"
#include "ComputeShader.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>


namespace Merlin::Tensor {

	ComputeShader::ComputeShader(std::string n) {
		_name = n;
		_xWkgrp = _yWkgrp = _zWkgrp = 1;
	}

	ComputeShader::~ComputeShader() {
		Delete();
	}

	void ComputeShader::Delete() {
		Console::trace("ComputeShader") << "Destructing Shader " << id() << Console::endl;
		if (compiled() != 0) {
			ShaderID = 0;
		}
	}

	void ComputeShader::PrintLimits() {
		// query limitations
	// -----------------
		int max_compute_work_group_count[3];
		int max_compute_work_group_size[3];
		int max_compute_work_group_invocations;

		for (int idx = 0; idx < 3; idx++) {
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
		}
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);

		int max_tex = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex);	

		Console::info("Renderer") << "OpenGL Limitations: " << Console::endl;
		Console::info("Renderer") << "Max texture size : " << max_tex << Console::endl;
		Console::info("Renderer") << "maxmimum number of work groups in X dimension " << max_compute_work_group_count[0] << Console::endl;
		Console::info("Renderer") << "maxmimum number of work groups in Y dimension " << max_compute_work_group_count[1] << Console::endl;
		Console::info("Renderer") << "maxmimum number of work groups in Z dimension " << max_compute_work_group_count[2] << Console::endl;

		Console::info("Renderer") << "maxmimum size of a work group in X dimension " << max_compute_work_group_size[0] << Console::endl;
		Console::info("Renderer") << "maxmimum size of a work group in Y dimension " << max_compute_work_group_size[1] << Console::endl;
		Console::info("Renderer") << "maxmimum size of a work group in Z dimension " << max_compute_work_group_size[2] << Console::endl;

		Console::info("Renderer") << "Number of invocations in a single local work group that may be dispatched to a compute shader " << max_compute_work_group_invocations << Console::endl;

	}

	void ComputeShader::Dispatch(GLuint x, GLuint y, GLuint z) {
		//glDispatchCompute(ceil(_width/8), ceil(_height/4), 1);

		_xWkgrp = x;
		_yWkgrp = y;
		_zWkgrp = z;
		Console::trace("ComputeShader") << "Dispatch: " << _xWkgrp << "x" << _yWkgrp << "x" << _zWkgrp << Console::endl;
		glDispatchCompute(_xWkgrp, _yWkgrp, _zWkgrp);
	}

	void ComputeShader::Compile(const std::string shader_file_path) {
		_compiled = true;
		ShaderID = glCreateShader(GL_COMPUTE_SHADER);

		// Read the Vertex ComputeShader code from the file
		LOG_INFO() << "Importing compute shader source... : " << shader_file_path << Console::endl;
		ShaderSrc = ReadSrc(shader_file_path);

		GLint Result = GL_FALSE;
		int InfoLogLength;

		// Compile Vertex ComputeShader
		//LOG << ("Compiling vertex shader...") << Console::endl;
		const char* ShaderSrcPointer = ShaderSrc.c_str();
		glShaderSource(ShaderID, 1, &ShaderSrcPointer, NULL);
		glCompileShader(ShaderID);

		// Check Vertex ComputeShader
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			Console::error() << &VertexShaderErrorMessage[0];
			Console::error() << &VertexShaderErrorMessage[0] << Console::endl;
			_compiled = false;
		}

		// Link the program
		//printf("Linking program\n");
		SetID(glCreateProgram());
		glAttachShader(id(), ShaderID);
		glLinkProgram(id());

		// Check the program
		glGetProgramiv(id(), GL_LINK_STATUS, &Result);
		glGetProgramiv(id(), GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(id(), InfoLogLength, NULL, &ProgramErrorMessage[0]);
			Console::error() << &ProgramErrorMessage[0] << Console::endl;
			_compiled = false;
		}
		Console::success("ComputeShader") << "shader " << shader_file_path << " compiled succesfully" << Console::endl;
		glDetachShader(id(), ShaderID);
		glDeleteShader(ShaderID);

	}

}
