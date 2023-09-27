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
		
		virtual void Execute(); //Execute using the WorkgroupLayout
		virtual void Dispatch(GLuint width, GLuint height = 1, GLuint layers = 1); //Execute using the given WorkgroupLayout
		void SetWorkgroupLayout(GLuint width, GLuint height = 1, GLuint layers = 1); // Set current workgroup Layout

		void PrintLimits();

		static Shared<ComputeShader> Create(const std::string& n, const std::string& file_path = "");
	protected:
		glm::uvec3 m_wkgrpLayout;
		GLuint m_shaderID = 0;
		std::string m_shaderSrc; 
	};

	class StagedComputeShader : public ComputeShader {
	public:
		StagedComputeShader(const std::string& n, GLuint numberOfStage, const std::string& file_path = "");
		~StagedComputeShader();

		void Execute() override; //Execute each stage of the shader using workGroupLayout
		void Execute(GLuint stage); //Execute a specific stage of the shader using workGroupLayout
		void Dispatch(GLuint width, GLuint height = 1, GLuint layers = 1) override; //Execute using the given WorkgroupLayout

		static Shared<StagedComputeShader> Create(const std::string& n, const std::string& file_path = "");
	protected:
		GLuint m_stage = 0;
	};


	class ComputeShaderLibrary {
	public:
		ComputeShaderLibrary();

		void Add(Shared<ComputeShader> shader);
		void Load(const std::string& filepath);
		const ComputeShader& Get(const std::string& name);
		Shared<ComputeShader> Share(const std::string& name);

		void Execute(const std::string& key);
		void Dispatch(const std::string& key, GLuint width, GLuint height = 1, GLuint layers = 1);
		bool Exists(const std::string& name);

	private:
		std::unordered_map<std::string, Shared<ComputeShader>> m_shaders;
	};

}


