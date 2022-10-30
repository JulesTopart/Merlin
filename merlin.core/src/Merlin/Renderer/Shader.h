#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include <memory>

#include "ShaderBase.h"

namespace Merlin::Renderer {

	class Shader : public ShaderBase{
	public:
		Shader(std::string n);
		~Shader();

		void Delete() override;
		void Compile(const std::string vertex_file_path,
					 const std::string fragment_file_path,
					 const std::string geometry_file_path = "");

	protected:

		GLuint vertexShaderID = 0;
		GLuint fragmentShaderID = 0;
		GLuint geometryShaderID = 0;

		std::string VertexShaderSrc, FragmentShaderSrc;
	};
}