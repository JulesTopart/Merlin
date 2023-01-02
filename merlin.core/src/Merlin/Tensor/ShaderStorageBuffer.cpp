#include "glpch.h"
#include "ShaderStorageBuffer.h"
#include "ParticleSystem.h"

namespace Merlin::Tensor {

	using namespace Renderer;


	void ShaderStorageBuffer::copy(Shared<ShaderStorageBuffer> origin, Shared<ShaderStorageBuffer> target, GLsizeiptr size) {
		//Copy the result in output buffer in the input buffer for next step
		origin->Bind(GL_COPY_READ_BUFFER);
		target->Bind(GL_COPY_WRITE_BUFFER);
		
		//Copy the ouput buffer into the input buffer
		glCopyBufferSubData(
			GL_COPY_READ_BUFFER,
			GL_COPY_WRITE_BUFFER,
			0,
			0,
			size
		);
	}

	ShaderStorageBuffer::ShaderStorageBuffer(std::string name) {
		// Generate a buffer object name.
		glGenBuffers(1, &_StorageID);
		_name = name;
		_size = 0;
		Bind();
	}

	ShaderStorageBuffer::~ShaderStorageBuffer() {
		// Generate a buffer object name.
		glDeleteBuffers(1, &_StorageID);
	}

	void ShaderStorageBuffer::SetBindingPoint(GLuint binding) {
		_binding = binding;
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

	void ShaderStorageBuffer::Attach(Shared<ShaderBase> sh){
		int block_index = glGetProgramResourceIndex(sh->id(), GL_SHADER_STORAGE_BLOCK, _name.c_str());
		glShaderStorageBlockBinding(sh->id(), block_index, _binding);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _binding, _StorageID);
	}
}