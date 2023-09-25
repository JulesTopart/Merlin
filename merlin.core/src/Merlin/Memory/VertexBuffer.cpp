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

	template<class T>
	VertexBuffer<T>::VertexBuffer(std::vector<T>& vertices) : BufferObject<T>(GL_ARRAY_BUFFER) {
		LoadData(vertices, GL_STATIC_DRAW);
		Console::trace("VertexBuffer") << "VertexBuffer " << this->m_bufferID << " allocated. " << Console::endl;
	}

	template<class T>
	VertexBuffer<T>::~VertexBuffer() {}
}