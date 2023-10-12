#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/ShaderBase.h"

namespace Merlin::Tensor{

	class ComputeShader : public Graphics::ShaderBase {
	public:
		ComputeShader(const std::string& n, const std::string& file_path = "");
		~ComputeShader();

		void Delete() override;
		void Compile(const std::string& file_path);
		void CompileFromSrc(const std::string& src);
		
		void Dispatch(); //Execute using the default WorkgroupLayout
		void Dispatch(GLuint width, GLuint height = 1, GLuint layers = 1); //Execute using the given WorkgroupLayout
		void SetWorkgroupLayout(GLuint width, GLuint height = 1, GLuint layers = 1); // Set current workgroup Layout

		void Wait();
		void Barrier(GLbitfield barrier = GL_ALL_BARRIER_BITS);

		void PrintLimits();

	protected:
		glm::uvec3 m_wkgrpLayout;
		GLuint m_shaderID = 0;
		std::string m_shaderSrc; 
	};

	class StagedComputeShader : public ComputeShader {
	public:
		StagedComputeShader(const std::string& n, const std::string& file_path, GLuint numberOfStage);

		void ExecuteAll();
		void Execute(GLuint stage);
		void Execute(GLuint start, GLuint end);
		void Step();

		inline void SetStageCount(GLuint n) { m_stageCount = n; }
		inline void SetStage(GLuint n) { m_stage = n; }
		inline void Reset() { m_stage = 0; }
		inline GLuint GetStage() { return m_stage; }
		inline GLuint GetStageCount() { return m_stageCount; }

	protected:
		GLuint m_stage = 0;
		GLuint m_stageCount = 0;

	};


	class ComputeShaderLibrary {
	public:
		ComputeShaderLibrary();

		void Add(Shared<ComputeShader> shader);
		void Load(const std::string& filepath);
		const ComputeShader& Get(const std::string& name);
		Shared<ComputeShader> Share(const std::string& name);

		void Dispatch(const std::string& key);
		void Dispatch(const std::string& key, GLuint width, GLuint height = 1, GLuint layers = 1);
		bool Exists(const std::string& name);

	private:
		std::unordered_map<std::string, Shared<ComputeShader>> m_shaders;
	};

	typedef Shared<ComputeShader> ComputeShader_Ptr;
	typedef Shared<StagedComputeShader> StagedComputeShader_Ptr;
}


