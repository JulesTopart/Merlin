#pragma once

#include "Merlin/Core/Core.h"

namespace Merlin::Memory{

	class BufferObject {
	public:
		BufferObject(GLenum target, bool BindBuffer = true);
		~BufferObject();

		void Bind();
		void BindAs(GLenum target);
		void Unbind();

		void Clear();

		template<typename T>
		void Allocate(std::vector<T> data, GLenum usage = GL_DYNAMIC_DRAW);

		template<typename T>
		void Allocate(GLsizeiptr size, GLenum usage = GL_DYNAMIC_DRAW);

		void PrintLimits();
		inline GLuint id() const { return _BufferID; }
		inline GLsizeiptr size() const { return _size; }

		void* Map();
		void Unmap();

		
	protected:
		
		std::string BufferObject::TypeToString();

		GLuint _BufferID; //OpenGL handle
		GLsizeiptr _size; //size in need in allocated memory (in bytes)
		GLenum _Target; //GL_ARRAY_BUFFER, GL_SHADER_STORAGE_BUFFER, ...
	};


	//Declare template function in the header to initialize when including
	template<typename T>
	void BufferObject::Allocate(std::vector<T> data, GLenum usage) {
		// Allocate storage for the buffer object.
		_size = data.size() * sizeof(T);
		glBufferData(_Target, _size, data.data(), usage);
	}

	template<typename T>
	void BufferObject::Allocate(GLsizeiptr count, GLenum usage) {

		std::vector<T> data;
		for (GLsizeiptr i(0); i < count; i++) {
			data.emplace_back();
		}
		Allocate<T>(data, usage);
	}

}


