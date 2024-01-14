#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Scene/Model.h"
#include "Merlin/Graphics/Mesh.h"
#include "Merlin/Util/Primitives.h"
#include "Merlin/Memory/IndexBuffer.h"
#include "Merlin/Util/Util.h"

using namespace Merlin::Graphics;
using namespace Merlin::Memory;

namespace Merlin::Utils{

	class ModelLoader {
	public:
		// Load a model from the specified file and return a pointer to a new Mesh object
		static Shared<Model> LoadModel(const std::string& filepath);

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
		static Vertex ParseVertex(const std::string& vertexString, const ModelData& objData);

		// Parse an OBJ file and extract the data
		static bool ParseOBJ(const std::string& file_path, Vertices& vertices, Indices& indices);

		// Parse an STL file and extract the data
		static bool ParseSTL(const std::string& file_path, Vertices& vertices, Indices& indices);
		static bool ParseSTL_ASCII(const std::string& file_path, Vertices& vertices, Indices& indices);
		static bool ParseSTL_BINARY(const std::string& file_path, Vertices& vertices, Indices& indices);



		//ParseGeometry
		//static bool ParseGEOM(const std::string& file_path, Vertices& vertices, Indices& indices);

		// Parse a any file and extract the data
		static bool ParseMesh(const std::string& file_path, Vertices& vertices, Indices& indices);


	};

}