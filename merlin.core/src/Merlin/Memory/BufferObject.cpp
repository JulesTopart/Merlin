#include "glpch.h"
#include "BufferObject.h"

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


	GenericBufferObject::GenericBufferObject(BufferTarget target, const std::string& name) : GLObject(create(), destroy) {
		m_target = target;
		m_bufferSize = 0;
		m_bindingPoint = m_bufferInstances;
		rename(name == "buffer" ? name + std::to_string(m_bufferInstances) : name);
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
		bind();
		Console::info("ShaderBase") << name() << "( block id " << id() << ") is now bound to binding point " << m_bindingPoint << Console::endl;
		glBindBufferBase(static_cast<GLenum>(m_target), m_bindingPoint, id());
	}


	void GenericBufferObject::bind() {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(static_cast<GLenum>(m_target), id());
	}

	void GenericBufferObject::bindAs(GLenum target) {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(static_cast<GLenum>(m_target), id());
	}

	void GenericBufferObject::unbind() {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(static_cast<GLenum>(m_target), 0);
	}

	void GenericBufferObject::clear() {
		GLubyte val = 0;
		glClearBufferData(static_cast<GLenum>(m_target), GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &val);
	}
}