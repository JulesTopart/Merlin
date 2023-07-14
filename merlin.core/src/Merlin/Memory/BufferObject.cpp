#include "glpch.h"
#include "BufferObject.h"


namespace Merlin::Memory {

	std::string BufferObject::TypeToString() {
		switch (_Target) {
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

	BufferObject::BufferObject(GLenum target, bool BindBuffer) {
		// Generate a buffer object name.
		glGenBuffers(1, &_BufferID);
		_Target = target;
		_size = 0;
		Console::trace("BufferObject") << TypeToString() << "Object" << _BufferID << " created. " << Console::endl;
		if(BindBuffer) Bind();
	}

	BufferObject::~BufferObject() {
		// Delete a buffer object name.
		glDeleteBuffers(1, &_BufferID);
		Console::trace("BufferObject") << TypeToString() << "Object" << _BufferID << " deleted. " << Console::endl;
	}

	void BufferObject::Bind() {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(_Target, _BufferID);
	}

	void BufferObject::BindAs(GLenum target) {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(target, _BufferID);
	}

	void BufferObject::Unbind() {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(_Target, 0);
	}

	void BufferObject::Clear() {
		GLubyte val = 0;
		glClearBufferData(_Target, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &val);
	}

	void BufferObject::PrintLimits() {
		// query limitations
	// -----------------
		int max_ubo;
		int max_ubo_size;
		int max_ubo_per_shader;
		int max_ssbo;
		int max_ssbo_size;

		glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_ubo);
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_ubo_size);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &max_ubo_per_shader);
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &max_ssbo);
		glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &max_ssbo_size);

		Console::info("Memory") << "OpenGL Limitations: " << Console::endl;
		Console::info("Memory") << "Max Uniform buffer object bindings : " << max_ubo << Console::endl;
		Console::info("Memory") << "Max Uniform buffer object block size : " << max_ubo << Console::endl;
		Console::info("Memory") << "Max Uniform buffer object per shader " << max_ubo_per_shader << Console::endl;
		Console::info("Memory") << "Max Shader Storage buffer object bindings : " << max_ssbo << Console::endl;
		Console::info("Memory") << "Max Shader Storage buffer object block size " << max_ssbo_size << Console::endl;


	}

	void* BufferObject::Map() {
		// Map the buffer object to a pointer.
		return glMapBuffer(_Target, GL_READ_WRITE);
	}

	void BufferObject::Unmap() {
		// Unmap the buffer object.
		glUnmapBuffer(_Target);
	}
}