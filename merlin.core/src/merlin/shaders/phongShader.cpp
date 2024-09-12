#include "glpch.h"
#include "phongShader.h"

namespace Merlin {

	Shared<PhongShader> PhongShader::create(const std::string& name,
		const std::string& vertex_file_path,
		const std::string& fragment_file_path,
		const std::string& geometry_file_path, bool compile) {

		return createShared<PhongShader>(name, vertex_file_path, fragment_file_path, geometry_file_path, compile);
	}

	PhongShader::PhongShader() : Shader() {}
	PhongShader::PhongShader(std::string n) : Shader(n) {

		supportEnvironment(true);
		supportLights(true);
		supportMaterial(true);
		supportShadows(true);
		supportTexture(true);
	}


	PhongShader::PhongShader(std::string n,
		const std::string vpath,
		const std::string fpath,
		const std::string gpath, bool compile) : Shader(n, vpath, fpath, gpath) {

		supportEnvironment(true);
		supportLights(true);
		supportMaterial(true);
		supportShadows(true);
		supportTexture(true);
	}

}