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
			const std::string gpath = "", bool compile = true);

		~Shader();

		void destroy() override;

		
		void compile();

		void readFile(const std::string& vertex_file_path,
			const std::string& fragment_file_path,
			const std::string& geometry_file_path = "");

		void compileFromFile(const std::string& vertex_file_path,
			const std::string& fragment_file_path,
			const std::string& geometry_file_path = "");

		void compileFromSrc(const std::string& vertex_src,
			const std::string& fragment_src,
			const std::string& geometry_src = "");


		static Shared<Shader> create(	const std::string& name,
										const std::string& vertex_file_path,
										const std::string& fragment_file_path,
										const std::string& geometry_file_path = "", bool compile = true);


		inline bool supportTexture() const		{ return _supportTexture; }
		inline bool supportMaterial() const		{ return _supportMaterial; }
		inline bool supportLights() const		{ return _supportLights; }
		inline bool supportShadows() const		{ return _supportShadows; }
		inline bool supportEnvironment() const	{ return _supportEnvironment; }

		inline void supportTexture(bool state) { _supportTexture = state; }
		inline void supportMaterial(bool state) { _supportMaterial = state; }
		inline void supportLights(bool state) { _supportLights = state; }
		inline void supportShadows(bool state) { _supportShadows = state; }
		inline void supportEnvironment(bool state) { _supportEnvironment = state; }


	protected:


		bool _supportLights = false;
		bool _supportTexture = false;
		bool _supportShadows = false;
		bool _supportMaterial = false;
		bool _supportEnvironment = false;

		GLuint vertexShaderID = 0;
		GLuint fragmentShaderID = 0;
		GLuint geometryShaderID = 0;

		std::string VertexShaderSrc = "";
		std::string FragmentShaderSrc = "";
		std::string GeomShaderSrc = "";
	};

	typedef Shared<Shader> Shader_Ptr;
}

