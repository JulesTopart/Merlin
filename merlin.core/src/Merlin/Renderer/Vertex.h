#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace Merlin::Renderer {

	class VertexBufferLayout;

	struct Vertex {
		glm::vec3
			position = glm::vec3(0.0f, 0.0f, 0.0f),
			normal = glm::vec3(0.0f, 0.0f, 0.0f),
			color = glm::vec3(0.0f, 0.0f, 0.0f);

		glm::vec2
			texCoord = glm::vec2(0.0f, 0.0f);

		static VertexBufferLayout GetLayout(); //Keep that static
	};

	typedef std::vector<Vertex> Vertices;
}