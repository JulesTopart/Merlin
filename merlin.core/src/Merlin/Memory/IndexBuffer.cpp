
#include "glpch.h"
#include "IndexBuffer.h"
#include "../Core/Console.h"

#include <glad/glad.h>

namespace Merlin::Memory {
	IndexBuffer::IndexBuffer(std::vector<GLuint>& indices) : BufferObject<GLuint>(GL_ELEMENT_ARRAY_BUFFER){
		LoadData(indices, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer() {}
}