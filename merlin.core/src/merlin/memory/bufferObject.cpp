#include "glpch.h"
#include "bufferObject.h"
#include "bindingPointManager.h"

namespace Merlin {

	std::string GenericBufferObject::targetToString() {
		switch (m_target) {
		case(BufferTarget::ARRAY_BUFFER):
			return "VertexBuffer";
			break;
		case(BufferTarget::ELEMENT_ARRAY_BUFFER):
			return "IndexBuffer";
			break;
		case(BufferTarget::QUERY_BUFFER):
			return "QueryBuffer";
			break;
		case(BufferTarget::TEXTURE_BUFFER):
			return "TextureBuffer";
			break;
		case(BufferTarget::UNIFORM_BUFFER):
			return "UniformBuffer";
			break;
		case(BufferTarget::SHADER_STORAGE_BUFFER):
			return "ShaderStorageBuffer";
			break;
		default:
			return "Buffer";
			break;
		}
	}

	GenericBufferObject::GenericBufferObject(BufferType type, size_t typeSize, BufferTarget target, const std::string& name) : GLObject(create(), destroy), m_type(type), m_typeSize(typeSize){
		m_target = target;
		m_bufferSize = 0;
		m_bindingPoint = 0;
		rename(name == "buffer" ? name + std::to_string(m_bufferInstances) : name);

		setBindingPoint();
		m_bufferInstances++;
	}

	GLuint GenericBufferObject::create() {
		GLuint ID;
		glCreateBuffers(1, &ID);
		return ID;
	}
	void GenericBufferObject::destroy(GLuint ID) {
		glDeleteBuffers(1, &ID);
	}

	void* GenericBufferObject::map() {
		// Map the buffer object to a pointer.
		return glMapBuffer(static_cast<GLenum>(m_target), GL_READ_WRITE);
	}

	void GenericBufferObject::unmap() {
		// Unmap the buffer object.
		glUnmapBuffer(static_cast<GLenum>(m_target));
	}

	void GenericBufferObject::setBindingPoint(GLuint bp) {
		m_bindingPoint = bp;
		Console::info("ShaderBase") << name() << "( block id " << id() << ") is now bound to binding point " << m_bindingPoint << Console::endl;
		glBindBufferBase(static_cast<GLenum>(m_target), m_bindingPoint, id());
	}

	void GenericBufferObject::setBindingPoint() {
		try {
			if (m_target == BufferTarget::ARRAY_BUFFER || m_target == BufferTarget::ELEMENT_ARRAY_BUFFER) {
				// For VBO and EBO, we might want to just track their usage but not bind them to indexed targets
				m_bindingPoint = BindingPointManager::instance().allocateBindingPoint(m_type);
			}
			else {
				m_bindingPoint = BindingPointManager::instance().allocateBindingPoint(m_type);
				Console::info("BufferBase") << name() << "( block id " << id() << ") has now binding point " << m_bindingPoint << Console::endl;
				glBindBufferBase(static_cast<GLenum>(m_target), m_bindingPoint, id());
			}
		}
		catch (const std::runtime_error& e) {
			Console::error("BindingPointManager") << e.what() << Console::endl;
		}
	}

	void GenericBufferObject::releaseBindingPoint() {
		if (m_bindingPoint != -1) {
			BindingPointManager::instance().releaseBindingPoint(m_type, m_bindingPoint);
			m_bindingPoint = -1;
		}
	}

	void GenericBufferObject::bind() {
		// bind the buffer object to the shader storage buffer target.
		glBindBuffer(static_cast<GLenum>(m_target), id());
		m_boundBuffers++;
	}

	void GenericBufferObject::bindAs(GLenum target) {
		// bind the buffer object to the shader storage buffer target.
		glBindBuffer(static_cast<GLenum>(m_target), id());
		m_boundBuffers++;
	}

	void GenericBufferObject::unbind() {
		// bind the buffer object to the shader storage buffer target.
		glBindBuffer(static_cast<GLenum>(m_target), 0);
		m_boundBuffers--;
	}

	void GenericBufferObject::resize(size_t size) {
		resizeRaw(size * m_typeSize);
	}

	void GenericBufferObject::reserve(size_t size, BufferUsage usage) {
		reserveRaw(size * m_typeSize, usage);
	}

	void GenericBufferObject::writeRaw(size_t bytesize, const void* data, BufferUsage usage) {
		if (usage != BufferUsage::DEFAULT_USAGE) m_usage = usage;
		if (bytesize != m_bufferSize) {
			m_bufferSize = bytesize;
			float gb, mb, kb = bytesize / 1000.0; mb = kb / 1000.0; gb = mb / 1000.0;
			Console::info("Buffer") << "allocating " << name().c_str() << " with " << int(m_bufferSize/m_typeSize) << " elements" << Console::endl;
			Console::info("Buffer") << "bound to binding point " << m_bindingPoint << Console::endl;
			if (gb > 0.5)		Console::info("Buffer") << "allocating " << gb << "GB of GPU Memory" << Console::endl;
			else if (mb > 0.5)	Console::info("Buffer") << "allocating " << mb << "MB of GPU Memory" << Console::endl;
			else if (kb > 0.5)	Console::info("Buffer") << "allocating " << kb << "kB of GPU Memory" << Console::endl;
			else Console::info("Buffer") << "allocating " << GLuint(m_bufferSize) << "bytes of GPU Memory" << Console::endl;
			glNamedBufferData(id(), m_bufferSize, data, static_cast<GLenum>(m_usage));
		}
		else {
			glNamedBufferSubData(id(), 0, m_bufferSize, data);
		}
	}

	void GenericBufferObject::writeSubRaw(size_t offset, size_t bytesize, const void* data) {
		if (offset + bytesize < m_bufferSize) {
			glNamedBufferSubData(id(), offset, bytesize, data);
		}
		else {
			Console::error("BufferObject") << "Invalid offset or size : out of range" << Console::endl;
		}
	}

	void GenericBufferObject::reserveRaw(size_t bytesize, BufferUsage usage) {
		writeRaw(bytesize, nullptr, usage);
		clear();
	}

	void GenericBufferObject::clear() {
		m_bufferSize = 0;
		glClearNamedBufferData(id(), GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, 0);
	}

	void GenericBufferObject::resizeRaw(GLuint byteSize){
		bindAs(GL_COPY_READ_BUFFER);
		recreate(create());
		bindAs(GL_COPY_WRITE_BUFFER);
		GLsizeiptr oldSize = size();
		reserveRaw(byteSize, m_usage);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, oldSize);
	}

	void GenericBufferObject::read(void* data) const {
		glGetNamedBufferSubData(id(), 0, m_bufferSize, data);
	}
	/*
	void GenericBufferObject::clear() {
		GLubyte val = 0;
		glClearBufferData(static_cast<GLenum>(m_target), GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &val);
	}*/



}