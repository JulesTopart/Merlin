#include "glpch.h"
#include "BufferObject.h"

namespace Merlin::Memory {

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
		m_bufferSize = 0;
		m_name = "buffer" + std::to_string(instances++);
		m_bufferID = -1;
		m_bindingPoint = instances++;
	}

	GenericBufferObject::GenericBufferObject(const std::string& name, GLenum target) {
		m_target = target;
		m_size = 0;
		m_bufferSize = 0;
		m_name = name;
		m_bufferID = -1;
		m_bindingPoint = instances++;
	}

	GenericBufferObject::~GenericBufferObject() {
		// Delete a buffer object name.
		Delete();
	}

	void GenericBufferObject::Rename(const std::string& name) {
		m_name = name;
	}

	void* GenericBufferObject::Map() {
		// Map the buffer object to a pointer.
		return glMapBuffer(m_target, GL_READ_WRITE);
	}

	void GenericBufferObject::Unmap() {
		// Unmap the buffer object.
		glUnmapBuffer(m_target);
	}

	void GenericBufferObject::SetBindingPoint(GLuint bp) {
		m_bindingPoint = bp;
		Bind();
		Console::info("ShaderBase") << name() << "( block id " << m_bufferID << ") is now bound to binding point " << m_bindingPoint << Console::endl;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_bindingPoint, m_bufferID);
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
			Console::trace("BufferObject") << TypeToString() << "Object" << m_bufferID << " deleted. Freed " << m_bufferSize / 1000000.0 << "Mb of device Memory" << Console::endl;
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
}