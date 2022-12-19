#pragma once
#include "../Renderer/Mesh.h"

using namespace Merlin::Renderer;

namespace Merlin::Utils{

	class ModelLoader {
	public:
		// Load a model from the specified file and return a pointer to a new Mesh object
		static std::shared_ptr<Mesh> LoadModel(const std::string& filepath);
		static std::shared_ptr<Mesh> LoadAxis(std::string name);
		static std::shared_ptr<Mesh> LoadCube(std::string name);

	private:

		enum class FileType {
			OBJ,
			STL,
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

		// Parse a any file and extract the data
		static bool ParseFile(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);


	};

}