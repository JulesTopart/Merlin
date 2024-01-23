#pragma once
#include "Merlin/Core/Core.h"

#include <string>
#include <memory>

#include "ShaderBase.h"

namespace Merlin::Graphics {

	class Shader : public ShaderBase {
	public:
		Shader();
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

		static Shared<Shader> Create(	const std::string& name,
										const std::string& vertex_file_path,
										const std::string& fragment_file_path,
										const std::string& geometry_file_path = "");

		inline bool SupportTexture() const		{ return _supportTexture; }
		inline bool SupportMaterial() const		{ return _supportMaterial; }
		inline void noMaterial()	{ _supportMaterial = false; }
		inline void noTexture()	{ _supportTexture = false; }

	protected:

		void CompileShader(const std::string& name, const std::string& src, GLuint id);


		bool _supportTexture = true;
		bool _supportMaterial = true;

		GLuint vertexShaderID = 0;
		GLuint fragmentShaderID = 0;
		GLuint geometryShaderID = 0;

		std::string VertexShaderSrc;
		std::string FragmentShaderSrc;
		std::string GeomShaderSrc;
	};

	typedef Shared<Shader> Shader_Ptr;
}

