#include "glpch.h"
#include "Merlin/Util/Primitives.h"
#include "Merlin/Memory/IndexBuffer.h"

namespace Merlin::Utils {

	Shared<Mesh> Primitives::CreateRectangle(float x, float y) {
		Vertices v = {
			Vertex{glm::vec3(-x / 2.0f,-y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
			Vertex{glm::vec3(-x / 2.0f, y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
			Vertex{glm::vec3(x / 2.0f, y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
			Vertex{glm::vec3(x / 2.0f,-y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
		};

		Indices i = { 0, 1, 2, 3 };
		return Mesh::Create("Rectangle", v, i, GL_QUADS);
	}

	Shared<Mesh> Primitives::CreateFloor(const int groundNumTiles, const float groundTileSize) {

		glm::vec2 squareVerts[] = { glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0) };

		float r = groundNumTiles / 2.0 * groundTileSize;

		std::vector<float> groundVerts;
		groundVerts.resize(3 * 4 * groundNumTiles * groundNumTiles, 0);
		std::vector<float> groundColors;
		groundColors.resize(3 * 4 * groundNumTiles * groundNumTiles, 0);

		for (int xi(0); xi < groundNumTiles; xi++) {
			for (int zi(0); zi < groundNumTiles; zi++) {
				float x = (-groundNumTiles / 2.0 + xi) * groundTileSize;
				float z = (-groundNumTiles / 2.0 + zi) * groundTileSize;
				float p = xi * groundNumTiles + zi;
				for(int i(0); i < 4; i++) {
					float q = 4 * p + i;
					float px = x + squareVerts[i][0] * groundTileSize;
					float pz = z + squareVerts[i][1] * groundTileSize;
					groundVerts[3 * q] = px;
					groundVerts[3 * q + 2] = pz;
					float col = 0.4;

					if((xi + zi) % 2 == 1){
						col = 0.9;
						float pr = sqrt(px * px + pz * pz);
						float d = std::max(0.0, 1.2 - pr / r);
						col = col * d;
						for(int j(0); j < 3; j++)
							groundColors[3 * q + j] = col;
						
					}
				}
			}
		}

		Vertices v;
		for (int i(0); i < 4 * groundNumTiles * groundNumTiles; i++) {
			v.push_back(Vertex{ glm::vec3(groundVerts[i * 3], groundVerts[i * 3 + 2], groundVerts[i * 3 + 1]),glm::vec3(0,0,1), glm::vec3(groundColors[i * 3], groundColors[i * 3 + 1], groundColors[i * 3 + 2]) });
		}

		return Mesh::Create("Floor", v, GL_QUADS);
	}

	Shared<Mesh> Primitives::CreatePoint() {
		Vertices v = {
			Vertex{glm::vec3(0,0,0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)}
		};
		return Mesh::Create("Point", v, GL_POINTS);
	}

	Shared<Mesh> Primitives::CreateLine(float length, glm::vec3 dir) {
		Vertices v = {
			Vertex{glm::vec3(0,0,0)},
			Vertex{glm::normalize(dir) * length}
		};

		return Mesh::Create("Line", v, GL_LINES);
	}

	Shared<Mesh> Primitives::CreateCoordSystem() {
		Vertices v = {
			Vertex{ glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
			Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
			Vertex{ glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(0.0f, 0.0f, 0.2f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
			Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)}
		};

		return Mesh::Create("CoordinateSystem", v, GL_LINES);
	}

	Shared<Mesh> Primitives::CreateQuadCube(float w, bool centered) {
		return CreateQuadCube(w, w, w, centered);
	}

	Shared<Mesh> Primitives::CreateQuadCube(float x, float y, float z, bool centered) {

		float x_lo;
		float x_up;
		float y_lo;
		float y_up;
		float z_lo;
		float z_up;

		if (centered) {
			x_lo = -x / 2.0f;
			x_up = x / 2.0f;
			y_lo = -y / 2.0f;
			y_up = y / 2.0f;
			z_lo = -z / 2.0f;
			z_up = z / 2.0f;
		}else{
			x_lo = 0;
			x_up = x;
			y_lo = 0;
			y_up = y;
			z_lo = 0;
			z_up = z;
		}

		std::vector<Vertex> v = {
			//   Coordinates
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f)}, // X-
			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},

			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f)}, // X+
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
	
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f)}, // Y-
			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},

			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f)}, // Y+
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},

			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f)}, // Z-
			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},

			Vertex{ glm::vec3(x_lo,y_lo,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f)}, // Z+
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)}

		};

		return Mesh::Create("Cube", v, GL_QUADS);
	}

	Shared<Mesh> Primitives::CreateCube(float w) { return CreateCube(w, w, w); }

	Shared<Mesh> Primitives::CreateCube(float x, float y, float z) {

		float x_lo = -x / 2.0f;
		float x_up = x / 2.0f;
		float y_lo = -y / 2.0f;
		float y_up = y / 2.0f;
		float z_lo = -z / 2.0f;
		float z_up = z / 2.0f;

		std::vector<Vertex> v = {
			//   Coordinates
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(0.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(0.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(0.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(0.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(0.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(0,0,-1),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(-1,0,0),glm::vec3(1), glm::vec2(0.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(0.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_up),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_lo),glm::vec3(0,-1,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(0.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_lo,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(0.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(0.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_lo),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(0.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(1,0,0),glm::vec3(1), glm::vec2(0.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_lo),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(0.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(1.0f, 0.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_lo),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(0.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(0,1,0),glm::vec3(1), glm::vec2(0.0f, 0.0f)},
			Vertex{ glm::vec3(x_up,y_up,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(1.0f, 1.0f)},
			Vertex{ glm::vec3(x_lo,y_up,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(0.0f, 1.0f)},
			Vertex{ glm::vec3(x_up,y_lo,z_up),glm::vec3(0,0,1),glm::vec3(1), glm::vec2(1.0f, 0.0f)}
		};

		return Mesh::Create("Cube", v);
	}

	Shared<Mesh> Primitives::CreateCone(float r, float h, int res) {
		const float angleStep = glm::two_pi<float>() / res;

		// Generate vertices
		Vertices vertices;
		vertices.reserve(res + 2);

		Vertex v = { glm::vec3(0.0f, h, 0.0f), glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.5f, 1.0f) };
		vertices.emplace_back(v);
		v = { glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f)), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.5f, 0.0f) };
		vertices.emplace_back(v);
		for (int i = 0; i < res; ++i) {
			const float angle = i * angleStep;
			const glm::vec3 position(r * sin(angle), 0.0f, r * cos(angle));
			const glm::vec3 normal = glm::normalize(glm::vec3(sin(angle), r / sqrt(r * r + h * h), cos(angle)));
			const glm::vec2 texCoord(angle / glm::two_pi<float>(), 0.0f);

			v = { position, normal, glm::vec3(1.0f, 1.0f, 1.0f), texCoord };

			vertices.emplace_back(v);
		}

		// Generate indices
		Indices indices;
		indices.reserve(res * 4);
		for (int i = 0; i < res; ++i) {
			const int i0 = 2 + i;
			const int i1 = 2 + (i + 1) % res;
			indices.push_back(0);
			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(1);
			indices.push_back(i1);
			indices.push_back(i0);
		}

		return CreateShared<Mesh>("Cone", vertices, indices, GL_TRIANGLES);
	}

	Shared<Mesh> Primitives::CreateCylinder(float r, float h, int res) {
		const float angleStep = glm::two_pi<float>() / res;

		// Generate indices
		Indices indices;
		indices.reserve(res * 12);
		for (int i = 0; i < res; ++i) {
			const int i0 = i * 2;
			const int i1 = i0 + 1;
			const int i2 = (i + 1) % res * 2;
			const int i3 = i2 + 1;
			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);
			indices.push_back(i2);
			indices.push_back(i1);
			indices.push_back(i3);

			// Indices for top face
			const int iTopCenter = (res + 1) * 2;
			indices.push_back(iTopCenter);
			indices.push_back(i0);
			indices.push_back(i2);

			// Indices for bottom face
			const int iBottomCenter = (res + 1) * 2 + 1;
			indices.push_back(iBottomCenter);
			indices.push_back(i3);
			indices.push_back(i1);
		}

		// Generate vertices
		Vertices vertices;
		vertices.reserve((res + 1) * 2 + 2);
		for (int i = 0; i <= res; ++i) {
			const float angle = i * angleStep;
			const glm::vec2 texCoordTop(angle / glm::two_pi<float>(), 1.0f);
			const glm::vec2 texCoordBottom(angle / glm::two_pi<float>(), 0.0f);
			vertices.emplace_back(glm::vec3(r * sin(angle), r * cos(angle), h), glm::normalize(glm::vec3(sin(angle), cos(angle), 0.0f)), glm::vec3(1.0f), texCoordTop);
			vertices.emplace_back(glm::vec3(r * sin(angle), r * cos(angle), 0.0f), glm::normalize(glm::vec3(sin(angle), cos(angle), 0.0f)), glm::vec3(1.0f), texCoordBottom);
		}
		// Add the center vertices for the top and bottom faces
		vertices.emplace_back(glm::vec3(0.0f, 0.0f, h), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(1.0f), glm::vec2(0.5f, 1.0f));
		vertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)), glm::vec3(1.0f), glm::vec2(0.5f, 0.0f));

		return CreateShared<Mesh>("Cylinder", vertices, indices, GL_TRIANGLES);
	}

	Shared<Mesh> Primitives::CreateSphere(float r, int hres, int vres) {

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;

		vertices.resize(hres * vres * 6);
		normals.resize(hres * vres * 6);

		float d_h = 2 * glm::pi<float>() / ((float)hres);
		float d_v = glm::pi<float>() / ((float)vres);
		int n = 0;
		// Vertices are created inside this loop.
		for (int i = 0; i < hres; i++) {
			float h = i * d_h;
			float hn = h + d_h;
			for (int j = 0; j < vres; j++) {
				float v = j * d_v;
				float vn = v + d_v;

				// The sphere is consists of multiple triangles where 2 triangles make a plane.
				// These 4 points are the corners of said plane. To create a triangle 3 of these corners are
				// used counterclockwise with the 2nd triangle's first point being the 1st last point.
				// Normal vectors are the same as the points without the radius multiplied.
				glm::vec4 p0(glm::cos(h) * glm::sin(v), glm::sin(h) * glm::sin(v),
					glm::cos(v), 1.0f);
				glm::vec4 p1(glm::cos(h) * glm::sin(vn), glm::sin(h) * glm::sin(vn),
					glm::cos(vn), 1.0f);
				glm::vec4 p2(glm::cos(hn) * glm::sin(v), glm::sin(hn) * glm::sin(v),
					glm::cos(v), 1.0f);
				glm::vec4 p3(glm::cos(hn) * glm::sin(vn), glm::sin(hn) * glm::sin(vn),
					glm::cos(vn), 1.0f);
				vertices[n] = p0 * r;
				normals.at(n++) = glm::vec3(p0);
				vertices.at(n) = p1 * r;
				normals.at(n++) = glm::vec3(p1);
				vertices.at(n) = p3 * r;
				normals.at(n++) = glm::vec3(p3);
				vertices.at(n) = p3 * r;
				normals.at(n++) = glm::vec3(p3);
				vertices.at(n) = p2 * r;
				normals.at(n++) = glm::vec3(p2);
				vertices.at(n) = p0 * r;
				normals.at(n++) = glm::vec3(p0);
			}
		}


		Vertices v;
		for (int i(0); i < vertices.size(); i++) {
			v.push_back({ vertices[i], normals[i], glm::vec3(1)});
		}
		return Mesh::Create("Sphere",v);
	}


	Shared<Mesh> Primitives::CreateFromQuad(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode) {
		std::vector<GLuint> indices_buffer;
		GLuint a; GLuint b;
		GLuint c; GLuint d;

		for (int i(0); i < indices.size(); i += 4) {
			a = indices_buffer[0]; b = indices_buffer[1];
			c = indices_buffer[2]; d = indices_buffer[3];

			indices_buffer.push_back(a - 1);//first triangle
			indices_buffer.push_back(b - 1);
			indices_buffer.push_back(c - 1);

			indices_buffer.push_back(a - 1);//second triangle
			indices_buffer.push_back(c - 1);
			indices_buffer.push_back(d - 1);
		}
		return Mesh::Create("QuadMesh", vertices, indices_buffer);
	}

}