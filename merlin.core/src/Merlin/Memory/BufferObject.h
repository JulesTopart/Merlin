#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Core/Console.h"

namespace Merlin::Memory{

	class GenericBufferObject {
	public :

		GenericBufferObject(GLenum target);
		GenericBufferObject(const std::string& name, GLenum target);
		~GenericBufferObject();

		void Rename(const std::string& name);
		void Generate(bool autoBind = true);
		void Delete();

		void Bind();
		void BindAs(GLenum target);
		void Unbind();

		void* Map();
		void Unmap();

		void Clear();

		inline GLuint id() const { return m_bufferID; }
		inline GLsizeiptr size() const { return m_size; }
		inline GLsizeiptr bufferSize() const { return m_bufferSize; }
		inline std::string name() const { return m_name; }

	protected:
		//Protected method
		std::string TypeToString();

		//Count buffer construction for named
		inline static unsigned int instances = 0;

		GLuint m_bufferID; //OpenGL handle
		GLsizeiptr m_size; //size in type instance
		GLsizeiptr m_bufferSize; //size in need in allocated memory (in bytes)
		GLenum m_target; //GL_ARRAY_BUFFER, GL_SHADER_STORAGE_BUFFER, ...
		std::string m_name;
		
	};


	template <class T>
	class BufferObject : public GenericBufferObject{
	public:
		BufferObject(GLenum target);
		BufferObject(const std::string& name, GLenum target);

		void Allocate(GLsizeiptr size, GLenum usage = GL_STATIC_DRAW, T* data = NULL);
		void LoadData(const std::vector<T>& data, GLenum usage = GL_STATIC_DRAW);

		void Resize(GLsizeiptr count);

		std::vector<T>& Download(); //Download the device Buffer onto host memory
		void Upload(); //Upload the host Buffer onto the device memory

		void FreeMemory(); //Clear the buffer and free device and host memory from content
		void FreeHostMemory(); //Clear the host memory buffer
		void FreeDeviceMemory(); //Clear the device memory buffer (You need to allocate it again then)
		
		void print();

		inline std::vector<T>& GetDeviceBuffer() { return m_cpuBuffer;}

	protected:
		GLenum m_usage;
		std::vector<T> m_cpuBuffer; //Buffer on host memory
	};



	template<class T>
	BufferObject<T>::BufferObject(GLenum target) : GenericBufferObject(target) {}

	template<class T>
	BufferObject<T>::BufferObject(const std::string& name, GLenum target) : GenericBufferObject(name, target) {}

	template<class T>
	void BufferObject<T>::Allocate(GLsizeiptr count, GLenum usage, T* data) {
		Generate(); //Generate and bind buffer
		m_usage = usage;
		m_size = count;
		m_bufferSize = count * sizeof(T);
		float kb = m_bufferSize / 1000.0;
		float mb = m_bufferSize / 1000000.0;
		float gb = m_bufferSize / 1000000000.0;

		Console::info("Buffer") << "allocating " << name().c_str() << " with " << int(m_size) << " objects" << Console::endl;
		if(gb > 0.5)		Console::info("Buffer") << "allocating " << gb << "GB of GPU Memory" << Console::endl;
		else if(mb > 0.5)	Console::info("Buffer") << "allocating " << mb << "MB of GPU Memory" << Console::endl;
		else if(kb > 0.5)	Console::info("Buffer") << "allocating " << kb << "kB of GPU Memory" << Console::endl;
		else Console::info("Buffer") << "allocating " << GLuint(m_bufferSize) << "bytes of GPU Memory" << Console::endl;

		
		glBufferData(m_target, m_bufferSize, data, usage);
		if (data == NULL) Clear(); //Initiliaze with zero
	}

	//Declare template function in the header to initialize when including
	template<class T>
	void BufferObject<T>::LoadData(const std::vector<T>& data, GLenum usage) {
		// Allocate storage for the buffer object.
		m_cpuBuffer = data;
		Allocate(m_cpuBuffer.size(), usage, m_cpuBuffer.data());
	}

	template<class T>
	void BufferObject<T>::Resize(GLsizeiptr size) {
		// Allocate storage for the buffer object.
		m_cpuBuffer.resize(size);
		m_size = size;
		m_bufferSize = size * sizeof(T);
		glBufferData(m_target, m_bufferSize, m_cpuBuffer.data(), m_usage);
	}

	template <class T>
	std::vector<T>& BufferObject<T>::Download() {
		m_cpuBuffer.resize(m_size);
		memcpy(m_cpuBuffer.data(), Map(), m_bufferSize);
		Unmap();
		return m_cpuBuffer;
	}

	template <class T>
	void BufferObject<T>::Upload() {
		if (m_cpuBuffer.size() > m_size) {
			Console::error("BufferObject") << "CPU buffer size is larger than device buffer. It can't be uploaded." << Console::endl;
		}
		else {
			Console::print() << m_cpuBuffer.size() * sizeof(T) << " | " << (long int)m_size << " | " << (long int)m_bufferSize << Console::endl;

			memcpy(Map(), m_cpuBuffer.data(), m_cpuBuffer.size() * sizeof(T));
			Unmap();
		}
	}


	template <class T>
	void BufferObject<T>::FreeHostMemory() {
		m_cpuBuffer = std::vector<T>();
	}

	template <class T>
	void BufferObject<T>::FreeDeviceMemory() {
		Delete();
	}


	template <class T>
	void BufferObject<T>::FreeMemory() {
		FreeHostMemory();
		FreeDeviceMemory();
	}


	template<typename T>
	void BufferObject<T>::print() {
		Bind();
		Download();

		Console::info("Buffer") << m_name << " : " << Console::endl << "[";
		for (GLuint i = 0; i < ((m_cpuBuffer.size() > 100) ? 100 : m_cpuBuffer.size() - 1); i++) {
			Console::print() << m_cpuBuffer[i] << ", ";
		}
		if (m_cpuBuffer.size() > 100) Console::print() << "..., ";
		Console::print() << m_cpuBuffer[m_cpuBuffer.size() - 1] << "]" << Console::endl << Console::endl;
	}

}