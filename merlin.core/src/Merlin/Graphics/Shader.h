#pragma once
#include "Merlin/Core/Core.h"

#include <string>
#include <memory>

#include "ShaderBase.h"

namespace Merlin::Graphics {

	class Shader : public ShaderBase {
	public:
		Shader(std::string n);
		Shader(std::string n,
			const std::string vpath,
			const std::string fpath,
			const std::string gpath = "");

		~Shader();

		void Delete() override;
		void Compile(const std::string& vertex_file_path,
			const std::string& fragment_file_path,
			const std::string& geometry_file_path = "");

		void CompileFromSrc(const std::string& vertex_src,
			const std::string& fragment_src,
			const std::string& geometry_src = "");

		static Shared<Shader> Create(const std::string& vertex_file_path,
			const std::string& fragment_file_path,
			const std::string& geometry_file_path = "");

	protected:

		void CompileShader(const std::string& name, const std::string& src, GLuint id);

		GLuint vertexShaderID = 0;
		GLuint fragmentShaderID = 0;
		GLuint geometryShaderID = 0;

		std::string VertexShaderSrc;
		std::string FragmentShaderSrc;
		std::string GeomShaderSrc;
	};


	class ShaderLibrary {
	public:
		static void Add(const std::string& name, const std::shared_ptr<Shader>& shader);
		static void Add(const std::shared_ptr<Shader>& shader);
		static std::shared_ptr<Shader> Load(const std::string& filepath);
		static std::shared_ptr<Shader> Get(const std::string& name);

		static bool Exists(const std::string& name);
	private:
		static std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
	};
}