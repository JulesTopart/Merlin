#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/bufferObject.h"
#include "merlin/shaders/shaderBase.h"

namespace Merlin {
	template<class T>
	class ShaderStorageBuffer : public BufferObject<T>{
	public:
		ShaderStorageBuffer();
		ShaderStorageBuffer(std::string name);
		ShaderStorageBuffer(std::string name, GLsizeiptr size, T* data, BufferUsage usage = BufferUsage::STATIC_DRAW);
		ShaderStorageBuffer(std::string name, GLsizeiptr count, BufferUsage usage = BufferUsage::STATIC_DRAW);
		~ShaderStorageBuffer();

		static void copy(Shared<ShaderStorageBuffer> origin, Shared<ShaderStorageBuffer> target, GLsizeiptr size);
		static Shared<ShaderStorageBuffer<T>> create(std::string name);
		static Shared<ShaderStorageBuffer<T>> create(std::string name, GLsizeiptr count, BufferUsage usage = BufferUsage::STATIC_DRAW);
		static Shared<ShaderStorageBuffer<T>> create(std::string name, GLsizeiptr size, T* data, BufferUsage usage = BufferUsage::STATIC_DRAW);

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
	ShaderStorageBuffer<T>::ShaderStorageBuffer() : BufferObject<T>(BufferType::SHADER_STORAGE_BUFFER, BufferTarget::SHADER_STORAGE_BUFFER) {}
	template <class T>
	ShaderStorageBuffer<T>::ShaderStorageBuffer(std::string name) : BufferObject<T>(BufferType::SHADER_STORAGE_BUFFER, BufferTarget::SHADER_STORAGE_BUFFER, name) {}
	template <class T>
	ShaderStorageBuffer<T>::ShaderStorageBuffer(std::string name, GLsizeiptr size, T* data, BufferUsage usage) : BufferObject<T>(BufferType::SHADER_STORAGE_BUFFER, BufferTarget::SHADER_STORAGE_BUFFER, name, size, data, usage) {}
	template <class T>
	ShaderStorageBuffer<T>::ShaderStorageBuffer(std::string name, GLsizeiptr count, BufferUsage usage) : BufferObject<T>(BufferType::SHADER_STORAGE_BUFFER, BufferTarget::SHADER_STORAGE_BUFFER, name, count, usage) {}

	template <class T>
	SSBO_Ptr<T> ShaderStorageBuffer<T>::create(std::string name) {
		return std::make_shared<ShaderStorageBuffer>(name);
	}

	template <class T>
	SSBO_Ptr<T> ShaderStorageBuffer<T>::create(std::string name, GLsizeiptr count, BufferUsage usage) {
		auto ptr = std::make_shared<ShaderStorageBuffer>(name, count, usage);
		return ptr;
	}

	template <class T>
	SSBO_Ptr<T> ShaderStorageBuffer<T>::create(std::string name, GLsizeiptr size, T* data, BufferUsage usage) {
		auto ptr = std::make_shared<ShaderStorageBuffer>(name, size, data, usage);
		return ptr;
	}

	template <class T>
	ShaderStorageBuffer<T>::~ShaderStorageBuffer() {}
}
