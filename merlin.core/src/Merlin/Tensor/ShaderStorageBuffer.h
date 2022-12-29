#pragma once
#include <glad/glad.h>
#include "Merlin/Renderer/ShaderBase.h"

namespace Merlin::Tensor {
	class ShaderStorageBuffer{
	public:
		ShaderStorageBuffer(GLuint binding_point);
		~ShaderStorageBuffer();

		void Allocate(GLsizeiptr size, const void* data);

		void Bind();
		void Bind(GLenum target);
		void Unbind();

		void* Map();
		void Unmap();

		void Attach(std::shared_ptr<Merlin::Renderer::ShaderBase> sh, std::string uniform);

		inline GLuint id() const { return _StorageID; }
		inline GLuint binding() const { return _binding; }

	private:
		GLuint _StorageID;
		GLuint _binding;
	};

	typedef ShaderStorageBuffer SSBO;
}

