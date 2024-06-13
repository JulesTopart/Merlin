#pragma once
#include "merlin/core/core.h"
#include "merlin/core/console.h"

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
		GenericBufferObject(BufferType type, size_t elementSize, BufferTarget target = BufferTarget::ARRAY_BUFFER, const std::string& name = "buffer"); //default constructor
		~GenericBufferObject() { releaseBindingPoint(); } //destructor

		void bind();
		void bindAs(GLenum target);
		void unbind();

		void setBindingPoint();
		void setBindingPoint(GLuint bp);
		void releaseBindingPoint();

		void* map();
		void unmap();
		void clear();
		void reserve(size_t size, BufferUsage usage = BufferUsage::DEFAULT_USAGE); //allcate space into device memory
		void resize(size_t size); //allcate space into device memory

		inline BufferTarget target() const { return m_target; }
		inline size_t size() const { return m_bufferSize; }
		inline GLuint bindingPoint() const { return m_bindingPoint; }

		void reserveRaw(size_t size, BufferUsage usage); //allocate space into device memory
		void writeRaw(size_t size, const void* data, BufferUsage = BufferUsage::DEFAULT_USAGE);
		void writeSubRaw(size_t offset, size_t size, const void* data);
		void resizeRaw(GLuint byteSize); //resize buffer while keeping content
		void read(void* data) const; //Read buffer from device memoryonto given array pointer


		//templates
	protected:
		template <typename T>
		void write(const std::vector<T>& data, BufferUsage = BufferUsage::DEFAULT_USAGE); //write data into device memory

		template <typename T>
		void writeSub(size_t offset, const std::vector<T>& data);

		template <typename T>
		std::vector<T> getEmptyArray() const; //Read buffer from device memory

		template <typename T>
		std::vector<T> read() const; //Read buffer from device memory

		template <typename T>
		void read(std::vector<T>& data) const; //Read buffer from device memory onto given vector array

		template <typename T>
		void print(); //print part of the array in console

	private:
		inline static unsigned int m_bufferInstances = 0;
		inline static unsigned int m_boundBuffers = 0;

		const BufferType m_type;
		BufferTarget m_target;			//Buffer target binding
		GLuint create();
		static void destroy(GLuint id);
		
	protected:
		std::string targetToString();

		size_t m_bufferSize = 0;//Number of bytes in buffer
		const size_t m_typeSize = 0;//Number of bytes of one element
		GLuint m_bindingPoint = 0;  //Current binding point
		BufferUsage m_usage = BufferUsage::STATIC_DRAW;
		
	};
	typedef Shared<GenericBufferObject> GenericBufferObject_Ptr;


	template <typename T>
	std::vector<T> GenericBufferObject::read() const {
		std::vector<T> result;
		result.resize(m_bufferSize / m_typeSize);
		read(result.data());
		return result;
	}

	template <typename T>
	std::vector<T> GenericBufferObject::getEmptyArray() const {
		return std::vector<T>();
	}

	template <typename T>
	void GenericBufferObject::read(std::vector<T>& data) const {
		read(data.data());
	}

	template <typename T>
	void GenericBufferObject::write(const std::vector<T>& data, BufferUsage usage){
		writeRaw(data.size() * m_typeSize, data.data(), usage);
	}

	template <typename T>
	void GenericBufferObject::writeSub(size_t offset, const std::vector<T>& data) {
		writeSubRaw(offset * m_typeSize, data.size() * m_typeSize, data.data());
	}

	template<typename T>
	void GenericBufferObject::print() {
		//bind();
		std::vector<T> cpuBuffer = read<T>();

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
		~BufferObject() {};

		BufferObject& operator=(const BufferObject& cpy) = delete; //avoid copying the GLObject
		BufferObject& operator=(BufferObject&& mov) noexcept = default; //allow to move the GLObject

		void write(const std::vector<T>& data, BufferUsage = BufferUsage::DEFAULT_USAGE); //write data into device memory
		void writeSub(size_t offset, const std::vector<T>& data);

		std::vector<T> getEmptyArray() const; //Read buffer from device memory
		std::vector<T> read() const; //Read buffer from device memory
		void read(std::vector<T>& data) const; //Read buffer from device memory onto given vector array
		void print(); //print part of the array in console
	};

	//Implementation
	template <class T>
	BufferObject(BufferTarget, const T*, size_t, BufferUsage) -> BufferObject<T>; //Template type deduction

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target) : GenericBufferObject(type, sizeof(T), target) {}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, const std::vector<T>& data, BufferUsage usage) : GenericBufferObject(type, sizeof(T), target) {
		m_usage = usage;
		write(data);
	}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, size_t count, BufferUsage usage) : GenericBufferObject(type, sizeof(T), target) {
		m_usage = usage;
		reserve(count);
	}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, size_t size, const T* data, BufferUsage usage) : GenericBufferObject(type, sizeof(T), target) {
		m_usage = usage;
		writeRaw(size, data, usage);
	}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, const std::string& name) : GenericBufferObject(type, sizeof(T), target, name) {}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, const std::string& name, const std::vector<T>& data, BufferUsage usage) : GenericBufferObject(type, sizeof(T), target, name) {
		m_usage = usage;
		write(data);
	}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, const std::string& name, size_t count, BufferUsage usage) : GenericBufferObject(type, sizeof(T), target, name) {
		m_usage = usage;
		reserve(count);
	}

	template<class T>
	BufferObject<T>::BufferObject(BufferType type, BufferTarget target, const std::string& name, size_t size, const T* data, BufferUsage usage) : GenericBufferObject(type, sizeof(T), target, name) {
		m_usage = usage;
		writeRaw(size * sizeof(T), data, usage);
	}





	// methods
	template <class T>
	std::vector<T> BufferObject<T>::read() const {
		return GenericBufferObject::read<T>(); 
	}

	template <class T>
	std::vector<T> BufferObject<T>::getEmptyArray() const {
		return std::vector<T>();
	}

	template <class T>
	void BufferObject<T>::read(std::vector<T>& data) const {
		GenericBufferObject::read<T>(data);
	}

	template <class T>
	void BufferObject<T>::write(const std::vector<T>& data, BufferUsage usage) {
		GenericBufferObject::write<T>(data, usage);
	}

	template <class T>
	void BufferObject<T>::writeSub(size_t offset, const std::vector<T>& data) {
		GenericBufferObject::writeSub<T>(offset, data);
	}

	template<class T>
	void BufferObject<T>::print() {
		GenericBufferObject::print<T>();
	}



}