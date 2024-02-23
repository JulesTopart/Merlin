#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/BufferObject.h"
#include "Merlin/Graphics/ShaderBase.h"

namespace Merlin {
	template<class T>
	class ShaderStorageBuffer : public BufferObject<T>{
	public:
		ShaderStorageBuffer();
		ShaderStorageBuffer(std::string name);
		ShaderStorageBuffer(std::string name, GLsizeiptr size, BufferUsage usage = BufferUsage::STATIC_DRAW, T* data = NULL);
		~ShaderStorageBuffer();

		static void copy(Shared<ShaderStorageBuffer> origin, Shared<ShaderStorageBuffer> target, GLsizeiptr size);
		static Shared<ShaderStorageBuffer<T>> Create(std::string name);
		static Shared<ShaderStorageBuffer<T>> Create(std::string name, GLsizeiptr size, BufferUsage usage = BufferUsage::STATIC_DRAW, T* data = NULL);

	};

	template<class T>
	using SSBO = ShaderStorageBuffer<T>;//Shorter alias
	template<class T>
	using SSBO_Ptr = Shared<ShaderStorageBuffer<T>>;//Shorter alias

	
	//todo pointers
	//Implementation
	template <class T>
	void ShaderStorageBuffer<T>::copy(Shared<ShaderStorageBuffer> origin, Shared<ShaderStorageBuffer> target, GLsizeiptr size) {
		//Copy the result in output buffer in the input buffer for next step
		origin->BindAs(GL_COPY_READ_BUFFER);
		target->BindAs(GL_COPY_WRITE_BUFFER);

		//Copy the ouput buffer into the input buffer
		glCopyBufferSubData(
			GL_COPY_READ_BUFFER,
			GL_COPY_WRITE_BUFFER,
			0,
			0,
			size
		);
	}

	template <class T>
	ShaderStorageBuffer<T>::ShaderStorageBuffer() : BufferObject<T>(BufferTarget::SHADER_STORAGE_BUFFER) {}
	template <class T>
	ShaderStorageBuffer<T>::ShaderStorageBuffer(std::string name) : BufferObject<T>(name, BufferTarget::SHADER_STORAGE_BUFFER) {}
	template <class T>
	ShaderStorageBuffer<T>::ShaderStorageBuffer(std::string name, GLsizeiptr size, BufferUsage usage, T* data) : BufferObject<T>(name, BufferTarget::SHADER_STORAGE_BUFFER, size, data, usage) {}

	template <class T>
	SSBO_Ptr<T> ShaderStorageBuffer<T>::Create(std::string name) {
		return std::make_shared<ShaderStorageBuffer>(name);
	}

	template <class T>
	SSBO_Ptr<T> ShaderStorageBuffer<T>::Create(std::string name, GLsizeiptr size, BufferUsage usage, T* data) {
		auto ptr = std::make_shared<ShaderStorageBuffer>(name, size, usage, data);
		return ptr;
	}


	template <class T>
	ShaderStorageBuffer<T>::~ShaderStorageBuffer() {}
}
