#include "glpch.h"
#include "ShaderStorageBuffer.h"

namespace Merlin::Tensor {

	using namespace Renderer;

	ShaderStorageBuffer::ShaderStorageBuffer(GLuint binding_point) {
		// Generate a buffer object name.
		glGenBuffers(1, &_StorageID);
		_binding = binding_point;

	}
	ShaderStorageBuffer::~ShaderStorageBuffer() {
		// Generate a buffer object name.
		glDeleteBuffers(1, &_StorageID);
	}

	void ShaderStorageBuffer::Bind() {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _StorageID);
	}
	void ShaderStorageBuffer::Bind(GLenum target) {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(target, _StorageID);
	}
	void ShaderStorageBuffer::Unbind() {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void ShaderStorageBuffer::Allocate(GLsizeiptr size, const void* data) {
		// Allocate storage for the buffer object.
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
	}

	void* ShaderStorageBuffer::Map() {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _StorageID);
		// Map the buffer object to a pointer.
		return glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	}

	void ShaderStorageBuffer::Unmap() {
		// Bind the buffer object to the shader storage buffer target.
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _StorageID);
		// Unmap the buffer object.
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}

	void ShaderStorageBuffer::Attach(std::shared_ptr<ShaderBase> sh, std::string buffername){
		int block_index = glGetProgramResourceIndex(sh->id(), GL_SHADER_STORAGE_BLOCK, buffername.c_str());
		glShaderStorageBlockBinding(sh->id(), block_index, _binding);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _binding, _StorageID);
	}
}