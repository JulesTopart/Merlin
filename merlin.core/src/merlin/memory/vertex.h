#pragma once
#include "merlin/core/core.h"

#include <vector>

namespace Merlin {

	class VertexBufferLayout;

	struct Vertex {
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 normal = glm::vec3(0.0f);
		glm::vec3 color = glm::vec3(1.0f);

		glm::vec2 texCoord = glm::vec2(0.0f, 0.0f);

		Vertex() {};
		Vertex(glm::vec3 position_,
			glm::vec3 normal_ = glm::vec3(0.0),
			glm::vec3 color_ = glm::vec3(1.0),
			glm::vec2 texCoord_ = glm::vec2(0.0)) :
			position(position_), normal(normal_), color(color_), texCoord(texCoord_) {};

		static VertexBufferLayout getLayout(); //Keep that static
	};

	typedef std::vector<Vertex> Vertices;
}