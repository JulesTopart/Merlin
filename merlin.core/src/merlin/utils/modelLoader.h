#pragma once
#include "merlin/core/core.h"
#include "merlin/scene/model.h"
#include "merlin/graphics/mesh.h"
#include "merlin/utils/primitives.h"
#include "merlin/memory/indexBuffer.h"
#include "merlin/utils/util.h"

namespace Merlin{

	class ModelLoader {
	public:
		// Load a model from the specified file and return a pointer to a new Mesh object
		static Shared<Model> loadModel(const std::string& filepath);

		// Load a GLTF file and extract the data
		//static Shared<Model> LoadGLTF(const std::string& file_path);

	private:

		struct Facet {
			glm::vec3 vA, vB, vC;
			glm::vec3 normal;
		};


		struct ModelData {
			std::vector<glm::vec3> vertices;
			std::vector<glm::vec2> texCoords;
			std::vector<glm::vec3> normals;
		};

		// Parse an Vertex Data from an OBJ
		static Vertex parseVertex(const std::string& vertexString, const ModelData& objData);

		// Parse an OBJ file and extract the data
		static bool parseOBJ(const std::string& file_path, Vertices& vertices, Indices& indices);

		// Parse an STL file and extract the data
		static bool parseSTL(const std::string& file_path, Vertices& vertices, Indices& indices);
		static bool parseSTL_ASCII(const std::string& file_path, Vertices& vertices, Indices& indices);
		static bool parseSTL_BINARY(const std::string& file_path, Vertices& vertices, Indices& indices);



		//ParseGeometry
		//static bool ParseGEOM(const std::string& file_path, Vertices& vertices, Indices& indices);

		//static void loadMTL(Shared<Model> mdl);
		// Parse a any file and extract the data
		static bool parseMesh(const std::string& file_path, Vertices& vertices, Indices& indices);



	};

}