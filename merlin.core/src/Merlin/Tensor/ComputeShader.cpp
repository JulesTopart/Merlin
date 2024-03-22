#include "glpch.h"
#include "ComputeShader.h"
#include "Merlin/Core/Log.h"

#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>
#include <vector>

#include <glm/gtc/type_ptr.hpp>

namespace Merlin {

	ComputeShader::ComputeShader(const std::string& n, const std::string& cspath) : ShaderBase(n, ShaderType::COMPUTE_SHADER){
		m_name = n;
		m_wkgrpLayout = glm::uvec3(1);
		if (cspath != "") compile(cspath);
	}

	ComputeShader::ComputeShader(const std::string& n, const std::string& cspath, ShaderType type) : ShaderBase(n, type) {
		m_name = n;
		m_wkgrpLayout = glm::uvec3(1);
		if (cspath != "") compile(cspath);
	}

	ComputeShader::~ComputeShader() {
		destroy();
	}

	void ComputeShader::destroy() {
		Console::trace("ComputeShader") << "Destructing Shader " << id() << Console::endl;
	}

	void ComputeShader::printLimits() {
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

	void ComputeShader::dispatch(GLuint x, GLuint y, GLuint z) {
		//Console::trace("ComputeShader") << "dispatch: " << int(x) << "x" << int(y) << "x" << int(z) << Console::endl;
		if (!isCompiled()) { Console::error("ComputeShader") << m_name << " is not Compiled" << Console::endl; return; }
		glDispatchCompute(x, y, z);
	}

	void ComputeShader::dispatch() {
		dispatch(m_wkgrpLayout.x, m_wkgrpLayout.y, m_wkgrpLayout.z);
	}

	void ComputeShader::wait() {
		glFinish();
	}

	void ComputeShader::barrier(GLbitfield barrier) {
		glMemoryBarrier(barrier);
	}

	void ComputeShader::SetWorkgroupLayout(GLuint x, GLuint y, GLuint z) {
		m_wkgrpLayout = glm::uvec3(x, y, z);
	}


	void ComputeShader::compile(const std::string& shader_file_path) {
		m_compiled = true;
		m_shaderID = glCreateShader(GL_COMPUTE_SHADER);

		// Read the Vertex ComputeShader code from the file
		LOG_INFO() << "Importing compute shader source... : " << shader_file_path << Console::endl;
		m_shaderSrc = readSrc(shader_file_path);
		compileFromSrc(m_shaderSrc);

	}


	void ComputeShader::compileFromSrc(const std::string& src) {
		m_compiled = true;
		m_shaderID = glCreateShader(GL_COMPUTE_SHADER);

		// Read the Vertex ComputeShader code from the file
		m_shaderSrc = src;

		GLint Result = GL_FALSE;
		int InfoLogLength;

		// Compile Vertex ComputeShader
		//LOG << ("Compiling vertex shader...") << Console::endl;
		const char* ShaderSrcPointer = m_shaderSrc.c_str();
		glShaderSource(m_shaderID, 1, &ShaderSrcPointer, NULL);
		glCompileShader(m_shaderID);

		// Check Vertex ComputeShader
		glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(m_shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(m_shaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			Console::error() << &VertexShaderErrorMessage[0];
			Console::error() << &VertexShaderErrorMessage[0] << Console::endl;
			m_compiled = false;
		}

		// Link the program
		//printf("Linking program\n");
		setID(glCreateProgram());
		glAttachShader(id(), m_shaderID);
		glLinkProgram(id());

		// Check the program
		glGetProgramiv(id(), GL_LINK_STATUS, &Result);
		glGetProgramiv(id(), GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(id(), InfoLogLength, NULL, &ProgramErrorMessage[0]);
			Console::error() << &ProgramErrorMessage[0] << Console::endl;
			m_compiled = false;
		}
		Console::success("ComputeShader") << "shader " << m_name << " compiled succesfully" << Console::endl;
		glDetachShader(id(), m_shaderID);
		glDeleteShader(m_shaderID);

	}


	ComputeShaderLibrary::ComputeShaderLibrary() {

		Shared<ComputeShader> defaultShader = createShared<ComputeShader>("default");

		std::string defaultSrc = R"( 
			#version 330 core
			layout (local_size_x = 1) in;
			void main() {}
		)";

		defaultShader->compileFromSrc(defaultSrc);
		//defaultShader->noMaterial();
		add(defaultShader);
	}

	
	void ComputeShaderLibrary::dispatch(const std::string& key) {
		if (!exist(key)) {
			Console::error("ComputeShaderLibrary") << "Shader " << key << " not found ! Using default shader instead." << Console::endl;
			return;
		}
		resources[key]->dispatch();
	}
	void ComputeShaderLibrary::dispatch(const std::string& key, GLuint width, GLuint height, GLuint layers) {
		if (!exist(key)) {
			Console::error("ComputeShaderLibrary") << "Shader " << key << " not found ! Using default shader instead." << Console::endl;
			return;
		}
		resources[key]->dispatch(width, height, layers);
	}


	StagedComputeShader::StagedComputeShader(const std::string& n, const std::string& file_path, GLuint numberOfStage) : ComputeShader(n, file_path, ShaderType::STAGED_COMPUTE_SHADER) {
		m_stage = 0;
		m_stageCount = numberOfStage;
	}

	void StagedComputeShader::executeAll() {
		m_stage = 0;
		while (m_stage < m_stageCount) step();
	}
	void StagedComputeShader::step() {
		setUInt("stage", m_stage);
		dispatch(m_wkgrpLayout.x, m_wkgrpLayout.y, m_wkgrpLayout.z);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		m_stage++;
	}
	void StagedComputeShader::execute(GLuint i) {
		m_stage = i;
		step();
	}
	void StagedComputeShader::execute(GLuint s, GLuint e) {
		m_stage = s;
		while (m_stage < e)
			step();
	}

}
