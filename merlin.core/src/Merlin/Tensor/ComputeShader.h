#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/ShaderBase.h"

namespace Merlin::Tensor{

	class ComputeShader : public Graphics::ShaderBase {
	public:
		ComputeShader(std::string n, const std::string file_path = "");
		~ComputeShader();

		void Delete() override;
		void Compile(const std::string file_path);
		void Dispatch(GLuint width, GLuint height = 1, GLuint layers = 1);

		void PrintLimits();

		static Shared<ComputeShader> Create(std::string n, const std::string file_path = "");
	protected:
		GLuint _xWkgrp, _yWkgrp, _zWkgrp;
		GLuint ShaderID = 0;
		std::string ShaderSrc;
	};

}


