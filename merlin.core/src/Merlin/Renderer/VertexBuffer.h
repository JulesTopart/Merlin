#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace Merlin::Renderer {

	struct Vertex {
		glm::vec3
			position = glm::vec3(0.0f, 0.0f, 0.0f),
			normal = glm::vec3(0.0f, 0.0f, 0.0f),
			color = glm::vec3(0.0f, 0.0f, 0.0f);

		glm::vec2
			textureUV = glm::vec2(0.0f, 0.0f);
	};

	class VertexBuffer {
	public:
		VertexBuffer(std::vector<Vertex>& vertices);
		~VertexBuffer();

		void Bind();
		void Unbind();

	private:
		unsigned int m_RendererID;
	};

	typedef VertexBuffer VBO;

}