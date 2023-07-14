#include "glpch.h"
#include "UniformBuffer.h"


namespace Merlin::Memory {


	UniformBuffer::UniformBuffer(std::string name) : BufferObject(GL_UNIFORM_BUFFER) {
		// Generate a buffer object name.
		_name = name;
		_binding = 0;
		Bind();
	}

	UniformBuffer::~UniformBuffer(){
	}

	void UniformBuffer::SetBindingPoint(GLuint binding) {
		_binding = binding;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _binding, _BufferID);
	}

	void UniformBuffer::Attach(const ShaderBase& sh) {
		int block_index = glGetProgramResourceIndex(sh.id(), GL_SHADER_STORAGE_BLOCK, _name.c_str());
		if (block_index == -1) Console::error("SSBO") << "Block " << _name << " not found in shader '" << sh.name() << "'. Did you bind it properly ?" << Console::endl;
		else {
			glShaderStorageBlockBinding(sh.id(), block_index, _binding);
			//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _binding, _BufferID);
		}
	}

	void UniformBuffer::copy(Shared<UniformBuffer> origin, Shared<UniformBuffer> target, GLsizeiptr size) {
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

}