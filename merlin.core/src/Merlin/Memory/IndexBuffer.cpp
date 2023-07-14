
#include "glpch.h"
#include "IndexBuffer.h"
#include "../Core/Console.h"

#include <glad/glad.h>

namespace Merlin::Memory {
	IndexBuffer::IndexBuffer(std::vector<GLuint>& indices) : BufferObject(GL_ELEMENT_ARRAY_BUFFER){
		Bind();
		// Give our vertices to OpenGL.
		Allocate<GLuint>(indices, GL_STATIC_DRAW);
		Console::trace("IndexBuffer") << "IndexBuffer " << _BufferID << " allocated. " << Console::endl;

	}

	IndexBuffer::~IndexBuffer() {}
}