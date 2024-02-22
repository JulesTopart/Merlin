#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/BufferObject.h"
#include "Merlin/Graphics/ShaderBase.h"

namespace Merlin {

	template<class T>
	class UniformBuffer : public BufferObject<T>{
		UniformBuffer(std::string name);
		~UniformBuffer();

		void Attach(const ShaderBase& sh, GLuint bindingPoint); //Link the program buffer to the right binding point
		static void copy(Shared<UniformBuffer> origin, Shared<UniformBuffer> target, GLsizeiptr size);

	};

	template<class T>
	UniformBuffer<T>::UniformBuffer(std::string name) : BufferObject<T>(GL_UNIFORM_BUFFER) {}

	template<class T>
	UniformBuffer<T>::~UniformBuffer() {
	}

	template<class T>
	void UniformBuffer<T>::Attach(const ShaderBase& sh, GLuint bindingPoint) {
		int block_index = glGetProgramResourceIndex(sh.id(), GL_SHADER_STORAGE_BLOCK, m_name.c_str());
		if (block_index == -1) Console::error("SSBO") << "Block " << m_name << " not found in shader '" << sh.name() << "'. Did you bind it properly ?" << Console::endl;
		else {
			glShaderStorageBlockBinding(sh.id(), block_index, bindingPoint);
		}
	}

	template<class T>
	void UniformBuffer<T>::copy(Shared<UniformBuffer> origin, Shared<UniformBuffer> target, GLsizeiptr size) {
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

	template<class T>
	using UBO = UniformBuffer<T>;
}


