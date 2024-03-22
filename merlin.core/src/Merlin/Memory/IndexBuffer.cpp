
#include "glpch.h"
#include "IndexBuffer.h"
#include "../Core/Console.h"

namespace Merlin {
	IndexBuffer::IndexBuffer(std::vector<GLuint>& indices) : BufferObject(BufferType::INDEX_BUFFER, BufferTarget::ELEMENT_ARRAY_BUFFER , indices, BufferUsage::STATIC_DRAW) {}
	IndexBuffer::~IndexBuffer() {}
}