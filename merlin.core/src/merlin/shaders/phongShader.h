#pragma once
#include "shader.h"


namespace Merlin {
	class PhongShader : public Shader{
	public:
		PhongShader();
		PhongShader(std::string n);
		PhongShader(std::string n,
			const std::string vpath,
			const std::string fpath,
			const std::string gpath = "", bool compile = true);

		static Shared<PhongShader> create(const std::string& name,
			const std::string& vertex_file_path,
			const std::string& fragment_file_path,
			const std::string& geometry_file_path = "", bool compile = true);



	};

	typedef Shared<PhongShader> PhongShader_Ptr;
}

