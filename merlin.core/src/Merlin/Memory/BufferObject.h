#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Core/Console.h"

namespace Merlin::Memory{

	class GenericBufferObject {
	public :

		GenericBufferObject(GLenum target);
		GenericBufferObject(const std::string& name, GLenum target);

		void Rename(const std::string& name);
		void Generate(bool autoBind = true);
		void Delete();

		void Bind();
		void BindAs(GLenum target);
		void Unbind();

		void Clear();

		inline GLuint id() const { return m_bufferID; }
		inline GLsizeiptr size() const { return m_size; }
		inline std::string name() const { return m_name; }

	private:
		//Protected method
		std::string TypeToString();

		//Count buffer construction for named
		inline static unsigned int instances = 0;

		GLuint m_bufferID; //OpenGL handle
		GLsizeiptr m_size; //size in need in allocated memory (in bytes)
		GLenum m_target; //GL_ARRAY_BUFFER, GL_SHADER_STORAGE_BUFFER, ...
		std::string m_name;
	};


	template <class T>
	class BufferObject : public GenericBufferObject{
	public:
		BufferObject(GLenum target);
		BufferObject(const std::string& name, GLenum target);

		void Allocate(GLsizeiptr size, GLenum usage = GL_STATIC_DRAW, T* data = NULL);
		void LoadData(std::vector<T>& data, GLenum usage = GL_STATIC_DRAW);

		void* Map();
		void Unmap();
		std::vector<T>& Sync(); //Download the GPU Buffer onto host memory

		void FreeMemory(); //Clear the buffer and free device and host memory from content
		void FreeHostMemory(); //Clear the host memory buffer
		void FreeDeviceMemory(); //Clear the device memory buffer (You need to allocate it again then)
		

		void print();

	protected:

		std::vector<T> m_cpuBuffer; //Buffer on host memory
	};


	std::string GenericBufferObject::TypeToString() {
		switch (m_target) {
		case(GL_ARRAY_BUFFER):
			return "VertexBuffer";
			break;
		case(GL_ELEMENT_ARRAY_BUFFER):
			return "IndexBuffer";
			break;
		case(GL_QUERY_BUFFER):
			return "QueryBuffer";
			break;
		case(GL_TEXTURE_BUFFER):
			return "TextureBuffer";
			break;
		case(GL_UNIFORM_BUFFER):
			return "UniformBuffer";
			break;
		case(GL_SHADER_STORAGE_BUFFER):
			return "ShaderStorageBuffer";
			break;
		default:
			return "Buffer";
			break;
		}
	}

	GenericBufferObject::GenericBufferObject(GLenum target) {
		m_target = target;
		m_size = 0;
		m_name = "buffer" + std::to_string(instances++);
		m_bufferID = -1;
	}

	GenericBufferObject::GenericBufferObject(const std::string& name, GLenum target) {
		m_target = target;
		m_size = 0;
		m_name = name;
		m_bufferID = -1;
	}

	GenericBufferObject::~GenericBufferObject() {
		// Delete a buffer object name.
		Delete();
	}

	void GenericBufferObject::Rename(const std::string& name) {
		m_name = name;
	}

	void GenericBufferObject::Generate(bool autoBind) {
		if (m_bufferID == -1) {
			// Generate a buffer object if not yet generated.
			glGenBuffers(1, &m_bufferID);
			Console::trace("BufferObject") << TypeToString() << "Object" << m_bufferID << " created. " << Console::endl;
		}
		if (autoBind) Bind();
	}

	void GenericBufferObject::Delete() {
		if (m_bufferID != -1) {
			// Delete buffer object if exist.
			glDeleteBuffers(1, &m_bufferID);
			Console::trace("BufferObject") << TypeToString() << "Object" << m_bufferID << " deleted. Freed " << m_size / 1000000.0 << "Mb of device Memory" << Console::endl;
		}
	}

	void GenericBufferObject::Bind() {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(m_target, m_bufferID);
	}

	void GenericBufferObject::BindAs(GLenum target) {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(target, m_bufferID);
	}

	void GenericBufferObject::Unbind() {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(m_target, 0);
	}

	void GenericBufferObject::Clear() {
		GLubyte val = 0;
		glClearBufferData(m_target, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &val);
	}

	template<class T>
	BufferObject<T>::BufferObject(GLenum target) : GenericBufferObject(target){}

	template<class T>
	BufferObject<T>::BufferObject(const std::string& name, GLenum target) : GenericBufferObject(name, target){}

	template<class T>
	void BufferObject<T>::Allocate(GLsizeiptr count, GLenum usage, T* data) {
		Generate(); //Generate and bind buffer
		m_size = count * sizeof(T);
		Console::info("Buffer") << "allocating " << m_size / 1000000.0 << "Mb of GPU Memory" << Console::endl;
		glBufferData(m_target, m_size, data, usage);
		if (data == NULL) Clear(); //Initiliaze with zero
	}

	//Declare template function in the header to initialize when including
	template<class T>
	void BufferObject<T>::LoadData(std::vector<T>& data, GLenum usage) {
		// Allocate storage for the buffer object.
		Allocate(data.size(), usage, data.data());
	}

	template <class T>
	std::vector<T>& BufferObject<T>::Sync() {
		m_cpuBuffer.resize(m_size / sizeof(T));
		memcpy(m_cpuBuffer.data(), Map(), m_size);
		Unmap();
		return m_cpuBuffer;
	}

	template <class T>
	void* BufferObject<T>::Map() {
		// Map the buffer object to a pointer.
		return glMapBuffer(m_target, GL_READ_WRITE);
	}

	template <class T>
	void BufferObject<T>::Unmap() {
		// Unmap the buffer object.
		glUnmapBuffer(m_target);
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
		Sync();

		Console::info("Buffer") << m_name << " : " << Console::endl << "[";
		for (GLuint i = 0; i < ((m_cpuBuffer.size() > 100) ? 100 : m_cpuBuffer.size() - 1); i++) {
			Console::print() << m_cpuBuffer[i] << ", ";
		}
		if (m_cpuBuffer.size() > 100) Console::print() << "..., ";
		Console::print() << m_cpuBuffer[m_cpuBuffer.size() - 1] << "]" << Console::endl << Console::endl;
	}

}