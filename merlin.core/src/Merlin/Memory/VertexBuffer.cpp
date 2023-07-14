#include "glpch.h"
#include "VertexBuffer.h"
#include "../Core/Console.h"

#include <glad/glad.h>

namespace Merlin::Memory{

	VertexBufferLayout Vertex::GetLayout() {
		VertexBufferLayout layout;
		layout.Push<float>(3); //Vertex pos
		layout.Push<float>(3); //Vertex normal
		layout.Push<float>(3); //Vertex color
		layout.Push<float>(2); //Texture coordinates
		return layout;
	}

	VertexBuffer::VertexBuffer(std::vector<float>& vertices) : BufferObject(GL_ARRAY_BUFFER) {
		Bind();
		Allocate<float>(vertices, GL_STATIC_DRAW);
		Console::trace("VertexBuffer") << "VertexBuffer " << _BufferID << " allocated. " << Console::endl;
	}

	VertexBuffer::VertexBuffer(std::vector<Vertex>& vertices) : BufferObject(GL_ARRAY_BUFFER) {
		Bind();
		Allocate<Vertex>(vertices, GL_STATIC_DRAW);
		Console::trace("VertexBuffer") << "VertexBuffer " << _BufferID << " allocated. " << Console::endl;
	}

	VertexBuffer::~VertexBuffer() {}
}