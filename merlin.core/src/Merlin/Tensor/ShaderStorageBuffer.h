#pragma once
#include <glad/glad.h>
#include "Merlin/Renderer/ShaderBase.h"

using namespace Merlin::Renderer;

namespace Merlin::Tensor {

	
	class ShaderStorageBuffer{
	public:
		ShaderStorageBuffer(std::string name);
		~ShaderStorageBuffer();

		template<typename T>
		void Allocate(std::vector<T> data);
		template<typename T>
		void Allocate(GLsizeiptr size);

		void Bind();
		void Bind(GLenum target);
		void Unbind();

		void* Map();
		void Unmap();
		void SetBindingPoint(GLuint binding);
		void Attach(Shared<ShaderBase> sh);

		inline GLuint id() const { return _StorageID; }
		inline GLuint binding() const { return _binding; }
		inline std::string name() const { return _name; }

		static void copy(Shared<ShaderStorageBuffer> origin, Shared<ShaderStorageBuffer> target, GLsizeiptr size);

	private:
		std::string _name;
		GLsizeiptr _size;
		GLuint _StorageID;
		GLuint _binding = 0;
	};

	template<typename T>
	void ShaderStorageBuffer::Allocate(std::vector<T> data) {
		// Allocate storage for the buffer object.
		_size = data.size() * sizeof(T);
		glBufferData(GL_SHADER_STORAGE_BUFFER, _size, data.data(), GL_DYNAMIC_DRAW);
	}

	template<typename T>
	void ShaderStorageBuffer::Allocate(GLsizeiptr count) {
		std::vector<T> data;
		for (GLsizeiptr i(0); i < count; i++) {
			data.emplace_back();
		}
		Allocate<T>(data);
	}

	typedef ShaderStorageBuffer SSBO;
}

