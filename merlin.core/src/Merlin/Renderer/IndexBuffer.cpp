
#include "glpch.h"
#include "IndexBuffer.h"
#include "../Core/Console.h"

#include <glad/glad.h>

namespace Merlin::Renderer {
	IndexBuffer::IndexBuffer(std::vector<GLuint>& indices) {
		glCreateBuffers(1, &m_RendererID);
		Bind();
		// Give our vertices to OpenGL.
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
		Console::trace("IndexBuffer") << "IndexBuffer " << m_RendererID << " created. " << Console::endl;

	}

	IndexBuffer::~IndexBuffer() {
		glDeleteBuffers(1, &m_RendererID);
		Console::trace("IndexBuffer") << "IndexBuffer " << m_RendererID << " deleted. " << Console::endl;
	}

	void IndexBuffer::Bind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);

	}

	void IndexBuffer::Unbind() {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}