#include "glpch.h"
#include "ModelLoader.h"
#include "Merlin/Core/Core.h"
#include "Merlin/Scene/Model.h"
#include "Merlin/Memory/IndexBuffer.h"
#include "Merlin/Memory/Vertex.h"

#include <tiny_gltf.h>


namespace Merlin::Utils {

	// Load a model from the specified file and return a pointer to a new Mesh object
    Shared<Model> ModelLoader::LoadModel(const std::string& file_path) {
        Vertices vertices;
        Indices indices;

        if (!ParseFile(file_path, vertices, indices)) {
            throw std::runtime_error("Unknown file type: " + file_path);
        }

        // Create a Mesh object using the parsed vertex and index data
        auto mesh = CreateShared<Mesh>(vertices, indices);
        mesh->RecalculateIndices();
        mesh->UpdateVAO();
        //mesh->RemoveUnusedVertices();
        //mesh->RecalculateNormals();   
        auto material = CreateShared<Material>();

        material->SetProperty(glm::vec3(0.2, 0.2, 0.2), glm::vec3(0.3, 0.3, 0.3), glm::vec3(0.3, 0.3, 0.3), 64);
        //material->SetShader(std::dynamic_pointer_cast<Shader>(ShaderLibrary::Get("default")));

        return Model::Create(mesh, material);
	}


    // Load a model from the specified file and return a pointer to a new Mesh object
    /*
    Shared<SceneNode> ModelLoader::LoadScene(const std::string& file_path) {

        if (Utils::GetFileType(file_path) != FileType::GLTF) {
            Console::error("ModelLoader") << "Cannot open " << file_path << " , LoadScene only support GLTF files" << Console::endl;
            return nullptr;
        }

        tinygltf::Model model;

        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, file_path);

        if (!ret) {
            Console::error("Failed to load GLTF file");
            return nullptr;
        }

        // You may need to implement functions to process the model's nodes, meshes, materials, textures, etc.
        // For now, let's assume there are functions ProcessNode, ProcessMesh, ProcessMaterial, ProcessTexture, etc.

        auto root = std::make_shared<SceneNode>("Root");

        // Iterate over the nodes in the GLTF model and create the corresponding SceneNodes
        for (const auto& gltfNode : model.nodes) {
            auto sceneNode = ProcessNode(gltfNode, model);
            root->AddChild(sceneNode);
        }
    }*/

    // Load a model from the specified file and return a pointer to a new Mesh object
    /*
    std::future<Shared<Model>> ModelLoader::LoadModelAsync(const std::string& file_path) {

        // Load model data in a background thread
        auto future = std::async(std::launch::async, [file_path]() {
            Vertices vertices;
        Indices indices;

        if (!ParseFile(file_path, vertices, indices)) {
            throw std::runtime_error("Unknown file type: " + file_path);
        }

        // Create a Mesh object using the parsed vertex and index data
        Shared<Mesh> mesh = CreateShared<Mesh>(vertices, indices);
        return Model::Create(mesh);

            });
        return future;
    }
    */
    Vertex ModelLoader::ParseVertex(const std::string& vertexString, const ModelData& objData) {
        // Use a stringstream to parse the vertex data
        std::stringstream ss(vertexString);
        std::string item;
        Vertex vertex;
        unsigned int index;

        // Extract the vertex index
        std::getline(ss, item, '/');
        index = std::stoul(item) - 1;
        vertex.position = objData.vertices[index];

        // Extract the texture coordinate index (if present)
        if (std::getline(ss, item, '/')) {
            index = std::stoul(item) - 1;
            vertex.texCoord = objData.texCoords[index];
        }

        // Extract the normal index (if present)
        if (std::getline(ss, item)) {
            index = std::stoul(item) - 1;
            vertex.normal = objData.normals[index];
        }

        return vertex;
    }

	// Parse an OBJ file and extract the data
	bool ModelLoader::ParseOBJ(const std::string& file_path, Vertices& vertices, Indices& indices) {
        // Open the OBJ file
        std::ifstream infile(file_path);
        if (!infile) {
            std::cerr << "Failed to open OBJ file: " << file_path << std::endl;
            return false;
        }

        // Temporary storage for the extracted data
        std::vector<glm::vec3> tempVertices;
        std::vector<glm::vec2> tempTexCoords;
        std::vector<glm::vec3> tempNormals;

        // Read the file line by line
        std::string line;
        while (std::getline(infile, line)) {
            if (line.empty() || line[0] == '#') {
                // Skip empty lines and comments
                continue;
            }

            // Use a stringstream to parse the line
            std::stringstream ss(line);
            std::string keyword;
            ss >> keyword;

            if (keyword == "v") {
                // Extract vertex data
                glm::vec3 vertex;
                ss >> vertex.x >> vertex.y >> vertex.z;
                tempVertices.push_back(vertex);
            }
            else if (keyword == "vt") {
                // Extract texture coordinate data
                glm::vec2 texCoord;
                ss >> texCoord.x >> texCoord.y;
                tempTexCoords.push_back(texCoord);
            }
            else if (keyword == "vn") {
                // Extract normal data
                glm::vec3 normal;
                ss >> normal.x >> normal.y >> normal.z;
                tempNormals.push_back(normal);
            }
            else if (keyword == "f") {
                // Extract face data
                std::string vertex1, vertex2, vertex3;
                ss >> vertex1 >> vertex2 >> vertex3;

                // Parse the vertex data for each of the vertices
                Vertex v1 = ParseVertex(vertex1, { tempVertices, tempTexCoords, tempNormals });
                Vertex v2 = ParseVertex(vertex2, { tempVertices, tempTexCoords, tempNormals });
                Vertex v3 = ParseVertex(vertex3, { tempVertices, tempTexCoords, tempNormals });

                // Add the vertices to the vector
                vertices.push_back(v1);
                vertices.push_back(v2);
                vertices.push_back(v3);

                // Add the indices to the vector
                indices.push_back(indices.size());
                indices.push_back(indices.size());
                indices.push_back(indices.size());
            }
        }

        return true;
	}

    bool ModelLoader::ParseSTL(const std::string& filepath, Vertices& vertices, Indices& indices) {
        // Open the file for reading
        std::ifstream file(filepath, std::ios::binary);

        // Check if the file was successfully opened
        if (!file.is_open()) {
            std::cerr << "Error: Failed to open file " << filepath << std::endl;
            return false;
        }

        // Read the first 80 bytes of the file (the size of the header in a binary STL file)
        char header[81];
        file.read(header, 80);
        header[80] = '\0';

        // Close the file
        file.close();

        // Check if the first 5 characters of the header are "solid"
        if (std::strncmp(header, "solid", 5) == 0) {
            // The file is an ASCII STL file
            return ParseSTL_ASCII(filepath, vertices, indices);
        }
        else {
            // The file is a binary STL file
            return ParseSTL_BINARY(filepath, vertices, indices);
        }
    }

    glm::vec3 computeFacetNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
        // Uses p2 as a new origin for p1,p3
        auto u = p2 - p1;
        auto v = p3 - p1;

        auto x = u.y * v.z - u.z * v.y;
        auto y = u.z * v.x - u.x * v.z;
        auto z = u.x * v.y - u.y * v.x;
        return glm::normalize(glm::vec3(x, y, z));
    }

    float angleBetween( glm::vec3 a, glm::vec3 b) {
        glm::vec3 da = glm::normalize(a);
        glm::vec3 db = glm::normalize(b);

        if (da == db) return 0.0;

        return glm::acos(glm::dot(da, db));
    }

	// Parse an STL file and extract the data
	bool ModelLoader::ParseSTL_BINARY(const std::string& file_path, Vertices& vertices, Indices& indices) {
        // Open the file in binary mode
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            // Failed to open the file
            return false;
        }

        // Read the header of the STL file
        uint8_t header[80];
        file.read((char*)&header, 80);

        // Read the number of triangles in the file
        uint32_t num_triangles;
        file.read((char*)&num_triangles, sizeof(num_triangles));

        // Reserve space in the vertex and index vectors
        vertices.reserve(num_triangles * 3);
        indices.reserve(num_triangles * 3);

        Console::info("ModelLoader") << "Loading Binary model..." << Console::endl;

        // Read the triangle data
        for (uint32_t i = 0; i < num_triangles; ++i) {
            // Read the normal vector
            float bn[3] = {0,0,0};
            file.read((char*)bn, sizeof(bn));

            // Read the three vertices
            float bv[9] = { 0,0,0 , 0,0,0, 0,0,0};
            file.read((char*)&bv, sizeof(bv));

            glm::vec3 v1, v2, v3;
            v1 = glm::vec3(bv[0], bv[1], bv[2]);
            v2 = glm::vec3(bv[3], bv[4], bv[5]);
            v3 = glm::vec3(bv[6], bv[7], bv[8]);

            glm::vec3 normal = computeFacetNormal(v1, v2, v3);

            Vertex vA, vB, vC;
            vA.position = v1;
            vB.position = v2; 
            vC.position = v3;
            vA.normal = normal;
            vB.normal = normal;
            vC.normal = normal;

            // Add the vertices to the vertex vector and update the indices
            unsigned int index = vertices.size();
            vertices.push_back(vA);
            vertices.push_back(vB);
            vertices.push_back(vC);
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(index + 2);

            // Read the attribute byte count (not used)
            int attr_byte_count;
            file.read((char*)&attr_byte_count, 2);
        }

        return true;
	}

    // Parse an STL file and extract the data
    bool ModelLoader::ParseSTL_ASCII(const std::string& file_path, Vertices& vertices, Indices& indices) {
        // Open the file in binary mode
        std::ifstream file(file_path, std::ios::in | std::ios::binary);
        if (!file) return false;
       
        // Reserve space for vertices and indices if you can estimate their size
        vertices.reserve(1024);
        indices.reserve(1024);

        std::vector<Facet> facets;
        facets.reserve(1024);

        Console::info("Model loader") << "Loading ascii model..." << Console::endl;


        // Read the file line by line
        std::string line; 
        Facet facetBuffer;

        float normal[3] = {0,0,0};

        int index = 0;
        while (std::getline(file, line)) {
            std::istringstream line_stream(line);
            std::string keyword;
            line_stream >> keyword;

            if (keyword[0] == 'f') { // facet
                // Read the normal vector
                std::string normal_keyword;
                line_stream >> normal_keyword;
                line_stream >> normal[0] >> normal[1] >> normal[2];
                
            }
            else if (keyword[0] == 'v') { // vertex
                Vertex v;
                line_stream >> v.position.x >> v.position.y >> v.position.z;
                v.normal = glm::vec3(normal[0], normal[1], normal[2]);
                vertices.push_back(v);
                indices.push_back(index++);
   
            }
        }



        return true;
    }
	

	// Parse a any file and extract the data
	bool ModelLoader::ParseFile(const std::string& file_path, Vertices& vertices, Indices& indices) {
		FileType file_type = GetFileType(file_path);
		switch (file_type) {
		case FileType::OBJ:
			return ParseOBJ(file_path, vertices, indices);
		case FileType::STL:
			return ParseSTL(file_path, vertices, indices);
        case FileType::GLTF:
            //return ParseGLTF(file_path, vertices, indices);
            return false;
		default:
			// Unknown file type
			return false;
		}
	}



}