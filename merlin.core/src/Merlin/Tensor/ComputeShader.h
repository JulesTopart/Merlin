#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/ShaderBase.h"
#include "Merlin/Graphics/RessourceManager.h"

namespace Merlin{

	class ComputeShader : public ShaderBase {
	public:
		ComputeShader(const std::string& n, const std::string& file_path = "");
		ComputeShader(const std::string& n, const std::string& file_path, ShaderType type);
		~ComputeShader();

		void destroy() override;
		void compile(const std::string& file_path);
		void compileFromSrc(const std::string& src);
		
		void dispatch(); //execute using the default WorkgroupLayout
		void dispatch(GLuint width, GLuint height = 1, GLuint layers = 1); //execute using the given WorkgroupLayout
		void SetWorkgroupLayout(GLuint width, GLuint height = 1, GLuint layers = 1); // Set current workgroup Layout

		void wait();
		void barrier(GLbitfield barrier = GL_ALL_BARRIER_BITS);

		void printLimits();

	protected:
		glm::uvec3 m_wkgrpLayout;
		GLuint m_shaderID = 0;
		std::string m_shaderSrc; 
	};

	class StagedComputeShader : public ComputeShader {
	public:
		StagedComputeShader(const std::string& n, const std::string& file_path, GLuint numberOfStage);

		void executeAll();
		void execute(GLuint stage);
		void execute(GLuint start, GLuint end);
		void step();

		inline void setStageCount(GLuint n) { m_stageCount = n; }
		inline void setStage(GLuint n) { m_stage = n; }
		inline void reset() { m_stage = 0; }
		inline GLuint getStage() const { return m_stage; }
		inline GLuint getStageCount() const { return m_stageCount; }

		static Shared<StagedComputeShader> create(const std::string& n, const std::string& file_path, GLuint numberOfStage) { return createShared<StagedComputeShader>(n, file_path, numberOfStage); };

	protected:
		GLuint m_stage = 0;
		GLuint m_stageCount = 0;

	};


	class ComputeShaderLibrary : public RessourceManager<ComputeShader>{
	public:
		ComputeShaderLibrary();
		inline void add(Shared<ComputeShader> shader) { RessourceManager::add(shader->name(), shader); };

		void dispatch(const std::string& key);
		void dispatch(const std::string& key, GLuint width, GLuint height = 1, GLuint layers = 1);

	};

	typedef Shared<ComputeShader> ComputeShader_Ptr;
	typedef Shared<StagedComputeShader> StagedComputeShader_Ptr;
}


