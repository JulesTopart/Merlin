#pragma once
#include "Merlin/Core/Core.h"

#include <vector>

namespace Merlin::Renderer {
	class IndexBuffer {
	public:
		IndexBuffer(std::vector<GLuint>& indices);
		~IndexBuffer();

		void Bind();
		void Unbind();

	private:
		unsigned int m_RendererID;
	};

	typedef IndexBuffer EBO;
	typedef std::vector<GLuint> Indices;
}