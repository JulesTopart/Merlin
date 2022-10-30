#include "glpch.h"
#include "VertexBuffer.h"
#include "../Core/Console.h"

#include <glad/glad.h>

namespace Merlin::Renderer {
	VertexBuffer::VertexBuffer(std::vector<Vertex>& vertices) {
		glCreateBuffers(1, &m_RendererID);
		// The following commands will talk about our 'vertexbuffer' buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		// Give our vertices to OpenGL.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		Console::trace("VertexBuffer") << "VertexBuffer " << m_RendererID << " created. " << Console::endl;

	}

	VertexBuffer::~VertexBuffer() {
		glDeleteBuffers(1, &m_RendererID);
		Console::trace("VertexBuffer") << "VertexBuffer " << m_RendererID << " deleted. " << Console::endl;
	}

	void VertexBuffer::Bind() {
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

	}

	void VertexBuffer::Unbind() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}