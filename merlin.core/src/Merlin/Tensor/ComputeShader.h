#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../Renderer/ShaderBase.h"

namespace Merlin::Tensor{

	class ComputeShader : public Renderer::ShaderBase {
	public:
		ComputeShader(std::string n);
		~ComputeShader();

		void Delete() override;
		void Compile(const std::string file_path);
		void Dispatch(GLuint width, GLuint height, GLuint layers = 1);

		void PrintLimits();
	protected:
		GLuint _xWkgrp, _yWkgrp, _zWkgrp;
		GLuint ShaderID = 0;
		std::string ShaderSrc;
	};
}


