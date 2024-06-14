#pragma once
#include "merlin/core/core.h"

#include <string>
#include <memory>

#include "shaderBase.h"

namespace Merlin {

	class Shader : public ShaderBase {
	public:
		Shader();
		Shader(std::string n);
		Shader(std::string n,
			const std::string vpath,
			const std::string fpath,
			const std::string gpath = "");

		~Shader();

		void destroy() override;
		void compile(const std::string& vertex_file_path,
			const std::string& fragment_file_path,
			const std::string& geometry_file_path = "");

		void compileFromSrc(const std::string& vertex_src,
			const std::string& fragment_src,
			const std::string& geometry_src = "");

		static Shared<Shader> create(	const std::string& name,
										const std::string& vertex_file_path,
										const std::string& fragment_file_path,
										const std::string& geometry_file_path = "");

		inline bool supportTexture() const		{ return _supportTexture; }
		inline bool supportMaterial() const		{ return _supportMaterial; }
		inline bool supportLights() const		{ return _supportLights; }
		inline bool supportShadows() const		{ return _supportShadows; }
		inline bool supportEnvironment() const	{ return _supportEnvironment; }
		inline void noMaterial()	{ _supportMaterial = false; }
		inline void noTexture()	{ _supportTexture = false; }
		inline void noLights()	{ _supportLights = false; }
		inline void noShadows()	{ _supportShadows = false; }
		inline void noEnvironment()	{ _supportEnvironment = false; }

	protected:

		void compileShader(const std::string& name, const std::string& src, GLuint id);


		bool _supportLights = true;
		bool _supportTexture = true;
		bool _supportShadows = true;
		bool _supportMaterial = true;
		bool _supportEnvironment = true;

		GLuint vertexShaderID = 0;
		GLuint fragmentShaderID = 0;
		GLuint geometryShaderID = 0;

		std::string VertexShaderSrc;
		std::string FragmentShaderSrc;
		std::string GeomShaderSrc;
	};

	typedef Shared<Shader> Shader_Ptr;
}

