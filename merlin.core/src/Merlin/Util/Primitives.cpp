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
			v.push_back(Vertex{ glm::vec3(groundVerts[i * 3], groundVerts[i * 3 + 2], groundVerts[i * 3 + 1]),glm::vec3(0,0,1), glm::vec3(groundColors[i * 3], groundColors[i * 3 + 1], groundColors[i * 3 + 2]) }); //color : 
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
			indices.push_back(i2);
			indices.push_back(i0);
			indices.push_back(iTopCenter);
			

			// Indices for bottom face
			const int iBottomCenter = (res + 1) * 2 + 1;
			indices.push_back(i1);
			indices.push_back(i3);
			indices.push_back(iBottomCenter);
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


	/*
	Shared<Mesh> Primitives::CreateSphere(float r, int hres, int vres) {

		std::vector<glm::vec3> position;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> tex;

		
		float x, y, z, xy;                              // vertex position
		float nx, ny, nz, lengthInv = 1.0f / r;    // vertex normal
		float s, t;                                     // vertex texCoord

		float sectorStep = 2 * glm::pi<float>() / hres;
		float stackStep = glm::pi<float>() / vres;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= vres; ++i)
		{
			stackAngle = glm::pi<float>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
			xy = r * cosf(stackAngle);             // r * cos(u)
			z = r * sinf(stackAngle);              // r * sin(u)

			// add (sectorCount+1) vertices per stack
			// first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= hres; ++j)
			{
				sectorAngle = j * sectorStep;           // starting from 0 to 2pi

				// vertex position (x, y, z)
				x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
				y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

				position.push_back(glm::vec3(x, y, z));

				// normalized vertex normal (nx, ny, nz)
				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;
				normals.push_back(glm::vec3(nx, ny, nz));

				// vertex tex coord (s, t) range between [0, 1]
				s = (float)j / hres;
				t = (float)i / vres;
				tex.push_back(glm::vec2(s, t));
			}
		}
		
		std::vector<int> indices;
		int k1, k2;
		for (int i = 0; i < vres; ++i)
		{
			k1 = i * (hres + 1);     // beginning of current stack
			k2 = k1 + hres + 1;      // beginning of next stack

			for (int j = 0; j < hres; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (vres - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}


		

		
		position.resize(hres * vres * 6);
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

				// The sphere is consists of multiple triangles where 2 triangles make a quad.
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
				position[n] = p0 * r;
				normals.at(n++) = glm::vec3(p0);
				position.at(n) = p1 * r;
				normals.at(n++) = glm::vec3(p1);
				position.at(n) = p3 * r;
				normals.at(n++) = glm::vec3(p3);
				position.at(n) = p3 * r;
				normals.at(n++) = glm::vec3(p3);
				position.at(n) = p2 * r;
				normals.at(n++) = glm::vec3(p2);
				position.at(n) = p0 * r;
				normals.at(n++) = glm::vec3(p0);

				// vertex tex coord (s, t) range between [0, 1]
				float s = (float)j / hres;
				float t = (float)i / vres;
				tex.push_back(glm::vec2(s, t));
			}
		}

		Vertices v;
		for (int i(0); i < position.size(); i++) {
			v.push_back({ position[i], normals[i], glm::vec3(1) });
		}
		return Mesh::Create("Sphere", v);
		

		
		Vertices v;
		for (int i(0); i < position.size(); i++) {
			v.push_back({ position[i], normals[i], glm::vec3(1), tex[i]});
		}

		Indices i;
		for (int j(0); j < indices.size(); j++) {
			i.push_back(indices[j]);
		}

		return Mesh::Create("Sphere",v, i);
		
	}
	*/




	Shared<Mesh> Primitives::CreateSphere(float r, int hres, int vres) {

		std::vector<glm::vec3> position;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> tex;
		std::vector<GLuint> indices;

		glm::vec3 v0 = glm::vec3(0, r, 0);
		tex.push_back(glm::vec2(0, 0));
		position.push_back(v0);
		normals.push_back(glm::normalize(v0));

		for (int i = 0; i < vres - 1; i++)
		{
			auto phi = glm::pi<float>() * double(i + 1) / double(vres);
			for (int j = 0; j < hres; j++)
			{
				auto theta = 2.0 * glm::pi<float>() * double(j) / double(hres);
				auto x = std::sin(phi) * std::cos(theta) * r;
				auto y = std::cos(phi) * r;
				auto z = std::sin(phi) * std::sin(theta) * r;
				position.push_back(glm::vec3(x, y, z));
				normals.push_back(glm::normalize((glm::vec3(x, y, z))));

				// vertex tex coord (s, t) range between [0, 1]
				float s = (float)j / hres;
				float t = (float)i / vres;
				tex.push_back(glm::vec2(s, t));
			}
		}

		glm::vec3 v1 = glm::vec3(0, -r, 0);
		position.push_back(v1);
		normals.push_back(glm::normalize(v1));
		tex.push_back(glm::vec2(0, 1));
		GLuint iv1 = position.size() - 1;

		for (int i = 0; i < hres; ++i)
		{
			auto i0 = i + 1;
			auto i1 = (i + 1) % hres + 1;
			indices.push_back(0);
			indices.push_back(i1);
			indices.push_back(i0);
			i0 = i + hres * (vres - 2) + 1;
			i1 = (i + 1) % hres + hres * (vres - 2) + 1;
			indices.push_back(iv1);
			indices.push_back(i1);
			indices.push_back(i0);
		}


		for (int j = 0; j < vres - 2; j++)
		{
			auto j0 = j * hres + 1;
			auto j1 = (j + 1) * hres + 1;
			for (int i = 0; i < hres; i++)
			{
				auto i0 = j0 + i;
				auto i1 = j0 + (i + 1) % hres;
				auto i2 = j1 + (i + 1) % hres;
				auto i3 = j1 + i;

				indices.push_back(i0);
				indices.push_back(i1);
				indices.push_back(i2);
				
				indices.push_back(i0);
				indices.push_back(i3);
				indices.push_back(i2);
			}
		}


		Vertices v;
		for (int i(0); i < position.size(); i++) {
			v.push_back({ position[i], normals[i], glm::vec3(1), tex[i]});
		}

		Indices i;
		for (int j(0); j < indices.size(); j++) {
			i.push_back(indices[j]);
		}

		return Mesh::Create("Sphere",v, i);

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