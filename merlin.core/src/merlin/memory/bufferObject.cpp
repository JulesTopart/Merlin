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


	GenericBufferObject::GenericBufferObject(BufferType type, BufferTarget target, const std::string& name) : GLObject(create(), destroy) {
		m_type = type;
		m_target = target;
		m_bufferSize = 0;
		m_bindingPoint = 0;
		setBindingPoint();

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
				Console::info("BufferBase") << name() << "( block id " << id() << ") is now bound to binding point " << m_bindingPoint << Console::endl;
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

	void GenericBufferObject::clear() {
		GLubyte val = 0;
		glClearBufferData(static_cast<GLenum>(m_target), GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &val);
	}
}