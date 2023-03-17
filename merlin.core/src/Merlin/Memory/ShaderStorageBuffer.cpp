#include "glpch.h"
#include "ShaderStorageBuffer.h"

namespace Merlin::Memory {

	using namespace Graphics;


	void ShaderStorageBuffer::copy(Shared<ShaderStorageBuffer> origin, Shared<ShaderStorageBuffer> target, GLsizeiptr size) {
		//Copy the result in output buffer in the input buffer for next step
		origin->BindAs(GL_COPY_READ_BUFFER);
		target->BindAs(GL_COPY_WRITE_BUFFER);
		
		//Copy the ouput buffer into the input buffer
		glCopyBufferSubData(
			GL_COPY_READ_BUFFER,
			GL_COPY_WRITE_BUFFER,
			0,
			0,
			size
		);
	}

	ShaderStorageBuffer::ShaderStorageBuffer(std::string name) : BufferObject(GL_SHADER_STORAGE_BUFFER){
		// Generate a buffer object name.
		_name = name;
		_binding = 0;
		Bind();
	}

	ShaderStorageBuffer::~ShaderStorageBuffer() {}

	void ShaderStorageBuffer::SetBindingPoint(GLuint binding) {
		_binding = binding;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _binding, _BufferID);
	}

	void ShaderStorageBuffer::Attach(Shared<ShaderBase> sh){
		int block_index = glGetProgramResourceIndex(sh->id(), GL_SHADER_STORAGE_BLOCK, _name.c_str());
		if (block_index == -1) Console::error("SSBO") << "Block " << _name << " not found in shader '" << sh->name() <<  "'. Did you bind it properly ?" << Console::endl;
		else {
			glShaderStorageBlockBinding(sh->id(), block_index, _binding);
			//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _binding, _BufferID);
		}
	}
}