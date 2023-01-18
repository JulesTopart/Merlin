#pragma once
#include "Merlin/Core/Core.h"

#include <vector>

namespace Merlin::Renderer {

	class VertexBufferLayout;

	struct Vertex {
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

		glm::vec2 texCoord = glm::vec2(0.0f, 0.0f);

		static VertexBufferLayout GetLayout(); //Keep that static
	};

	typedef std::vector<Vertex> Vertices;
}