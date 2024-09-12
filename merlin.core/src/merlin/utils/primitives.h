#pragma once
#include "merlin/graphics/mesh.h"

namespace Merlin {
	class Primitives {
	public:
		// Load a model from the specified file and return a pointer to a new Mesh object
		static Shared<Mesh> createCircle(float r, int res = 20);
		static Shared<Mesh> createOutlinedCircle(float r, int res = 20);
		static Shared<Mesh> createRectangle(float x, float y);
		static Shared<Mesh> createQuadRectangle(float x, float y, bool centered = true);
		static Shared<Mesh> createFloor(const int groundNumTiles, const float groundTileSize);
		static Shared<Mesh> createPoint();
		static Shared<Mesh> createLine(float length, glm::vec3 dir = glm::vec3(1, 0, 0));
		static Shared<Mesh> createCoordSystem();
		static Shared<Mesh> createQuadCube(float w, bool centered = true);
		static Shared<Mesh> createQuadCube(float x, float y, float z, bool centered = true);
		static Shared<Mesh> createCube(float w);
		static Shared<Mesh> createCube(float x, float y, float z);
		static Shared<Mesh> createSphere(float r, int hres = 10, int vres = 10);
		static Shared<Mesh> createFromQuad(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode = GL_TRIANGLES);
		static Shared<Mesh> createCone(float r, float h, int res);
		static Shared<Mesh> createCylinder(float r, float h, int res);

	};

}
