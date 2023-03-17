#pragma once
#include "Merlin/Graphics/Mesh.h"

namespace Merlin::Utils {
	using namespace Merlin::Graphics;
	class Primitives {
	public:
		// Load a model from the specified file and return a pointer to a new Mesh object
		static Shared<Mesh> CreateRectangle(float x, float y);
		static Shared<Mesh> CreatePoint();
		static Shared<Mesh> CreateLine(float length, glm::vec3 dir = glm::vec3(1, 0, 0));
		static Shared<Mesh> CreateCoordSystem();
		static Shared<Mesh> CreateCube(float w);
		static Shared<Mesh> CreateCube(float x, float y, float z);
		static Shared<Mesh> CreateSphere(float r, int hres = 10, int vres = 10);
		static Shared<Mesh> CreateFromQuad(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode = GL_TRIANGLES);

	};

}
