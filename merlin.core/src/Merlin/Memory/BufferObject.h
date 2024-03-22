#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Core/Console.h"

#include <glm/glm.hpp>
#include <glad/gl.h>
#include <algorithm>

namespace Merlin{

	enum class BufferType {
		ABSTRACT_BUFFER,
		ARRAY_BUFFER,
		VERTEX_BUFFER,
		INDEX_BUFFER,
		SHADER_STORAGE_BUFFER,
		UNIFORM_BUFFER
	};

	enum class BufferTarget : GLenum {
		ARRAY_BUFFER = GL_ARRAY_BUFFER,
		ATOMIC_COUNTER_BUFFER = GL_ATOMIC_COUNTER_BUFFER,
		COPY_READ_BUFFER = GL_COPY_READ_BUFFER,
		COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER,
		DISPATCH_INDIRECT_BUFFER = GL_DISPATCH_INDIRECT_BUFFER,
		DRAW_INDIRECT_BUFFER = GL_DRAW_INDIRECT_BUFFER,
		ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
		PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER,
		PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER,
		QUERY_BUFFER = GL_QUERY_BUFFER,
		SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER,
		TEXTURE_BUFFER = GL_TEXTURE_BUFFER,
		TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER,
		UNIFORM_BUFFER = GL_UNIFORM_BUFFER
	};

	enum class BufferUsage : GLenum {
		DEFAULT_USAGE = 0,
		STREAM_DRAW = GL_STREAM_DRAW,
		STREAM_READ = GL_STREAM_READ,
		STREAM_COPY = GL_STREAM_COPY,
		STATIC_DRAW = GL_STATIC_DRAW,
		STATIC_READ = GL_STATIC_READ,
		STATIC_COPY = GL_STATIC_COPY,
		DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
		DYNAMIC_READ = GL_DYNAMIC_READ,
		DYNAMIC_COPY = GL_DYNAMIC_COPY
	};

	class GenericBufferObject : public GLObject<> {
	public :
		GenericBufferObject(BufferType type, BufferTarget target = BufferTarget::ARRAY_BUFFER, const std::string& name = "buffer"); //default constructor

		void bind();
		void bindAs(GLenum target);
		void unbind();

		void setBindingPoint(GLuint bp);

		void* map();
		void unmap();
		void clear();

		inline BufferTarget target() const { return m_target; }
		inline GLsizeiptr size() const { return m_size; }
		inline GLsizeiptr bufferSize() const { return m_bufferSize; }
		inline GLuint bindingPoint() const { return m_bindingPoint; }

	private:
		inline static unsigned int m_bufferInstances = 0;

		BufferType m_type;
		BufferTarget m_target;			//Buffer target binding
		GLuint create();
		static void destroy(GLuint id);
		
	protected:
		std::string targetToString();

		GLsizeiptr m_size = 0;//Number of bytes in buffer
		GLsizeiptr m_bufferSize = 0;//Number of bytes in buffer

		GLuint m_bindingPoint = 0;  //Current binding point
		
		
	};
	typedef Shared<GenericBufferObject> GenericBufferObject_Ptr;

	template <class T>
	class BufferObject : public GenericBufferObject{
	public:
		BufferObject(BufferType type, BufferTarget target = BufferTarget::ARRAY_BUFFER);
		BufferObject(BufferType type, BufferTarget target, const std::vector<T>&, BufferUsage usage);
		BufferObject(BufferType type, BufferTarget target, size_t count, BufferUsage usage);
		BufferObject(BufferType type, BufferTarget target, size_t size, const T* data, BufferUsage usage);

		BufferObject(BufferType type, BufferTarget target, const std::string& name);
		BufferObject(BufferType type, BufferTarget target, const std::string& name, const std::vector<T>&, BufferUsage usage);
		BufferObject(BufferType type, BufferTarget target, const std::string& name, size_t count, BufferUsage usage);
		BufferObject(BufferType type, BufferTarget target, const std::string& name, size_t size, const T* data, BufferUsage usage);

		BufferObject(const BufferObject& cpy) = delete;
		BufferObject(BufferObject&& mov) noexcept = default;

		BufferObject& operator=(const BufferObject& cpy) = delete; //avoid copying the GLObject
		BufferObject& operator=(BufferObject&& mov) noexcept = default; //allow to move the GLObject

		void clear(); //Clear the device memory buffer (You need to reserve it again then)

		void reserve(size_t size); //allcate space into device memory
		void reserveRaw(size_t size); //allcate space into device memory

		void write(const std::vector<T>& data, BufferUsage = BufferUsage::DEFAULT_USAGE); //write data into device memory
		void writeSub(size_t offset, const std::vector<T>& data);

		void writeRaw(size_t size, const T* data, BufferUsage = BufferUsage::DEFAULT_USAGE);
		void writeSubRaw(size_t offset, size_t size, const T* data);
		
		std::vector<T> getEmptyArray() const; //Read buffer from device memory
		std::vector<T> read() const; //Read buffer from device memory
		void read(T* data) const; //Read buffer from device memoryonto given array pointer
		void read(std::vector<T>& data) const; //Read buffer from device memory onto given vector array

		void print(); //print part of the array in console

	protected:
		BufferUsage m_usage = BufferUsage::STATIC_DRAW;
	};

	//Implementation
	template <class T>
	BufferObject(BufferTarget, const T*, size_t, BufferUsage) -> BufferObject<T>; //Template type deduction

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target) : GenericBufferObject(type, target) {}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, const std::vector<T>& data, BufferUsage usage) : GenericBufferObject(type, target) {
		m_usage = usage;
		write(data);
	}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, size_t count, BufferUsage usage) : GenericBufferObject(type, target) {
		m_usage = usage;
		reserve(count);
	}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, size_t size, const T* data, BufferUsage usage) : GenericBufferObject(type, target) {
		m_usage = usage;
		writeRaw(size, data, usage);
	}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, const std::string& name) : GenericBufferObject(type, target, name) {}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, const std::string& name, const std::vector<T>& data, BufferUsage usage) : GenericBufferObject(type, target, name) {
		m_usage = usage;
		write(data);
	}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, const std::string& name, size_t count, BufferUsage usage) : GenericBufferObject(type, target, name) {
		m_usage = usage;
		reserve(count);
	}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, const std::string& name, size_t size, const T* data, BufferUsage usage) : GenericBufferObject(type, target, name) {
		m_usage = usage;
		writeRaw(size, data, usage);
	}


	template <class T>
	std::vector<T> BufferObject<T>::read() const{
		std::vector<T> result;
		result.resize(m_bufferSize / sizeof(T));
		read(result.data());
		return result;
	}

	template <class T>
	std::vector<T> BufferObject<T>::getEmptyArray() const {
		return std::vector<T>();
	}

	template <class T>
	void BufferObject<T>::read(T* data) const {
		glGetNamedBufferSubData(id(), 0, m_bufferSize, data);
	}

	template <class T>
	void BufferObject<T>::read(std::vector<T>& data) const {
		read(data.data());
	}

	template <class T>
	void BufferObject<T>::reserve(size_t size) {
		auto buf = getEmptyArray();
		buf.resize(size);
		write(buf);
	}

	template <class T>
	void BufferObject<T>::reserveRaw(size_t size) {
		auto buf = getEmptyArray();
		buf.resize(size/sizeof(T));
		write(buf);
	}

	template <class T>
	void BufferObject<T>::writeRaw(size_t size, const T* data, BufferUsage usage) {
		if(usage != BufferUsage::DEFAULT_USAGE) m_usage = usage;
		if (size != m_bufferSize) {
			m_bufferSize = size;
			m_size = size / sizeof(T);
			float gb, mb, kb = size / 1000.0; mb = kb / 1000.0; gb = mb / 1000.0;
			Console::info("Buffer") << "allocating " << name().c_str() << " with " << int(m_size) << " elements" << Console::endl;
			if (gb > 0.5)		Console::info("Buffer") << "allocating " << gb << "GB of GPU Memory" << Console::endl;
			else if (mb > 0.5)	Console::info("Buffer") << "allocating " << mb << "MB of GPU Memory" << Console::endl;
			else if (kb > 0.5)	Console::info("Buffer") << "allocating " << kb << "kB of GPU Memory" << Console::endl;
			else Console::info("Buffer") << "allocating " << GLuint(m_bufferSize) << "bytes of GPU Memory" << Console::endl;
			glNamedBufferData(id(), size, data, static_cast<GLenum>(m_usage));
		}
		else {
			glNamedBufferSubData(id(), 0, size, data);
		}
	}

	template <class T>
	void BufferObject<T>::writeSubRaw(size_t offset, size_t size, const T* data) {
		if (offset + size < m_bufferSize) {
			glNamedBufferSubData(id(), offset, size, data);
		}
		else {
			Console::error("BufferObject") << "Invalid offset or size : out of range" << Console::endl;
		}
	}

	template <class T>
	void BufferObject<T>::write(const std::vector<T>& data, BufferUsage usage) {
		writeRaw(data.size() * sizeof(T), data.data(), usage);
	}

	template <class T>
	void BufferObject<T>::writeSub(size_t offset, const std::vector<T>& data){
		writeRaw(offset * sizeof(T), data.size() * sizeof(T), data.data());
	}


	template <class T>
	void BufferObject<T>::clear() {
		m_bufferSize = 0;
		m_size = 0;
		glNamedBufferData(id(), 0, nullptr, static_cast<GLenum>(m_usage));
	}


	template<typename T>
	void BufferObject<T>::print() {
		//bind();
		std::vector<T> cpuBuffer = read();

		Console::info("Buffer") << name() << " = (" << cpuBuffer.size() << ")[";
		for (GLuint i = 0; i < std::min(int(cpuBuffer.size()), 100); ++i) {
			Console::print() << cpuBuffer[i] << ", ";
		}
		if (cpuBuffer.size() > 100) {
			Console::print() << "..., ";
			Console::print() << cpuBuffer[cpuBuffer.size() - 1];
		}
		else if (cpuBuffer.empty()) Console::print() << "empty";
		Console::print() << "]" << Console::endl << Console::endl;
		
	}

}