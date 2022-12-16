#include "glpch.h"
#include "MeshLoader.h"
#include "../Renderer/Mesh.h"


/*
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

using namespace Merlin::Renderer;

namespace Merlin::Utils::MeshLoader {

	std::shared_ptr<Mesh> LoadObject(std::string path, std::string name) {
		Assimp::Importer importer;


		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

		if (scene == nullptr) {
			Console::error("Mesh Loader") << importer.GetErrorString();
			std::vector<Vertex> vertex_buffer_data_unindexed = {
				//             COORDINATES          /             NORMAL         /          COLOR          /        	TexCoord       /
				Vertex{glm::vec3(1.0f, 0.0f, 1.0f)},
				Vertex{glm::vec3(1.0f, 0.0f,-1.0f)},
				Vertex{glm::vec3(-1.0f, 0.0f,-1.0f)},
				Vertex{glm::vec3(1.0f, 0.0f, 1.0f)},
				Vertex{glm::vec3(-1.0f, 0.0f, -1.0f)},
				Vertex{glm::vec3(-1.0f, 0.0f, 1.0f)}
			};

			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(name);
			mesh->LoadVertex(vertex_buffer_data_unindexed);
			mesh->SetDrawMode(GL_TRIANGLE_STRIP);
			Console::error("SceneManager") << "Invalid Mesh, spawned a plane instead " << name << Console::endl;
			return mesh;
		}

		if (scene->HasMeshes()) {

			const aiMesh* paiMesh = scene->mMeshes[0];

			Console::trace("Mesh Loader") << name << " : " << path << "loaded successfully." << Console::endl;
			Console::info("Mesh Loader") << name << " contains " << paiMesh->mFaces->mNumIndices
				<< " indices and " << paiMesh->mNumVertices << " verticies" << Console::endl;

			

			if (paiMesh->HasFaces()) {
				std::vector<Vertex> Vertices;
				std::vector<GLuint> Indices;
				const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

				for (size_t i = 0; i < paiMesh->mNumVertices; i++) {
					const aiVector3D* pPos = &(paiMesh->mVertices[i]);
					const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
					const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

					Vertex v = { glm::vec3(pPos->x, pPos->y, pPos->z) / 100.0f,
								 glm::vec3(pNormal->x, pNormal->y, pNormal->z),
								 glm::vec3(1.0, 1.0, 1.0),
								 glm::vec2(pTexCoord->x, pTexCoord->y) };


					Console::info() << "Parsing data..."; Console::printProgress(float(i) / float(paiMesh->mNumVertices));
					Vertices.push_back(v);
				}
				Console::info() << "Done." << Console::endl;

				for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
					const aiFace& Face = paiMesh->mFaces[i];
					assert(Face.mNumIndices == 3);
					Indices.push_back(Face.mIndices[0]);
					Indices.push_back(Face.mIndices[1]);
					Indices.push_back(Face.mIndices[2]);
				}

				std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(name);
				mesh->LoadVertex(Vertices, Indices);
				mesh->SetDrawMode(GL_TRIANGLES);
				Console::info("SceneManager") << "Spawned Mesh : " << name << Console::endl;
				return mesh;

			}
		} 
	}
}

*/

namespace Merlin::Utils::MeshLoader {

	bool stringEndWith(std::string filepath, std::string extension) {
		if (filepath.size() >= extension.size() &&
			filepath.compare(filepath.size() - extension.size(), extension.size(), extension) == 0)
			return true;
		else
			return false;
	}

	
	SupportedFileType GetFileType(std::string filepath){
		if (stringEndWith(filepath, ".obj")) {
			return SupportedFileType::OBJ;
		}
		else if (stringEndWith(filepath, ".stl")) {
			return SupportedFileType::STL;
		}
		else if (stringEndWith(filepath, ".bin")) {
			return SupportedFileType::BINARY;
		}
		else return SupportedFileType::UNKNOWN;
	}

	/*
	std::shared_ptr<Renderer::Mesh> ImportMesh(std::string name, std::string filepath) {
		SupportedFileType ft = GetFileType(filepath); 
		
		switch (ft) {
		case SupportedFileType::STL :

			break;

		case SupportedFileType::OBJ:

			break;

		default:

			break;
		}

		return std::shared_ptr<Renderer::Mesh>("none");
	}*/
}