#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Scene/Model.h"
#include "Merlin/Graphics/Mesh.h"
#include "Primitives.h"

#include <thread>
#include <future>

using namespace Merlin::Graphics;
using namespace Merlin::Memory;

namespace Merlin::Utils{

	class ModelLoader {
	public:
		// Load a model from the specified file and return a pointer to a new Mesh object
		static Shared<Model> LoadModel(const std::string& filepath);
		static std::future<Shared<Model>> ModelLoader::LoadModelAsync(const std::string& file_path);

	private:

		struct Facet {
			glm::vec3 vA, vB, vC;
			glm::vec3 normal;
		};

		enum class FileType {
			OBJ,
			STL,
			GEOM,
			UNKNOWN
		};

		struct ModelData {
			std::vector<glm::vec3> vertices;
			std::vector<glm::vec2> texCoords;
			std::vector<glm::vec3> normals;
		};


		static FileType GetFileType(const std::string& filepath);
		static std::string GetFileExtension(const std::string& filepath);
		static std::string GetFileName(const std::string& filepath);

		// Parse an Vertex Data from an OBJ
		static Vertex ParseVertex(const std::string& vertexString, const ModelData& objData);

		// Parse an OBJ file and extract the data
		static bool ParseOBJ(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

		// Parse an STL file and extract the data
		static bool ParseSTL(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
		static bool ParseSTL_ASCII(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
		static bool ParseSTL_BINARY(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

		//ParseGeometry
		static bool ParseGEOM(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

		// Parse a any file and extract the data
		static bool ParseFile(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);


	};

}