#include "glpch.h"
#include "ModelLoader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>

#include <tiny_gltf.h>

#include "../Renderer/Vertex.h"

namespace Merlin::Utils {


    std::shared_ptr<Mesh> ModelLoader::LoadCube(std::string name) {
        
        std::vector<Vertex> vertices = {
            Vertex{ glm::vec3(-0.5f, -0.5f, -0.5f)},
            Vertex{ glm::vec3(0.5f, -0.5f, -0.5f)},
            Vertex{ glm::vec3(0.5f,  0.5f, -0.5f)},
            Vertex{ glm::vec3(-0.5f,  0.5f, -0.5f)},
            Vertex{ glm::vec3(-0.5f, -0.5f,  0.5f)},
            Vertex{ glm::vec3(0.5f, -0.5f,  0.5f)},
            Vertex{ glm::vec3(0.5f,  0.5f,  0.5f)},
            Vertex{ glm::vec3(-0.5f,  0.5f,  0.5f)} 
        };

        std::vector<GLuint> indices = {
            0, 1, 3, 3, 1, 2,
            1, 5, 2, 2, 5, 6,
            5, 4, 6, 6, 4, 7,
            4, 0, 7, 7, 0, 3,
            3, 2, 7, 7, 2, 6,
            4, 5, 0, 0, 5, 1
        };

        std::shared_ptr<Mesh> cube = std::make_shared<Mesh>("Cube");
        cube->LoadVertex(vertices, indices);
        cube->SetDrawMode(GL_TRIANGLES);
        return cube;
    }


    std::shared_ptr<Mesh> ModelLoader::LoadPlane(std::string name) {
        
        //Plane
        std::vector<Vertex> vertices = {
            //             COORDINATES          /             NORMAL         /          COLOR          /        	TexCoord       /
            Vertex{ glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
            Vertex{ glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
            Vertex{ glm::vec3( 1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
            Vertex{ glm::vec3( 1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
        };

        std::vector<GLuint> indices = {
            0, 1, 2,
            0, 2, 3
        };

        std::shared_ptr<Mesh> plane = std::make_shared<Mesh>("Plane");
        plane->LoadVertex(vertices, indices);
        plane->SetDrawMode(GL_TRIANGLES);
        return plane;
    }

    std::shared_ptr<Mesh> ModelLoader::LoadAxis(std::string name) {
        //Axis
        std::vector<Vertex> vertices = {
            Vertex{ glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
            Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
            Vertex{ glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
            Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
            Vertex{ glm::vec3(0.0f, 0.0f, 0.2f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
            Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)}
        };

        std::shared_ptr<Mesh> axis = std::make_shared<Mesh>("Axis");
        axis->LoadVertex(vertices);
        axis->SetDrawMode(GL_LINES);
        return axis;
    }

	// Load a model from the specified file and return a pointer to a new Mesh object
    std::shared_ptr<Mesh> ModelLoader::LoadModel(const std::string& file_path) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        if (!ParseFile(file_path, vertices, indices)) {
            throw std::runtime_error("Unknown file type: " + file_path);
        }

        // Create a Mesh object using the parsed vertex and index data
        auto mesh = std::make_shared<Mesh>(GetFileName(file_path));
        mesh->LoadVertex(vertices, indices);
        return mesh;
	}


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
	bool ModelLoader::ParseOBJ(const std::string& file_path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
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

    bool ModelLoader::ParseSTL(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
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


	// Parse an STL file and extract the data
	bool ModelLoader::ParseSTL_BINARY(const std::string& file_path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
        // Open the file in binary mode
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            // Failed to open the file
            return false;
        }

        // Read the header of the STL file
        char header[80];
        file.read(header, 80);

        // Read the number of triangles in the file
        uint32_t num_triangles;
        file.read((char*)&num_triangles, sizeof(num_triangles));

        // Reserve space in the vertex and index vectors
        vertices.reserve(num_triangles * 3);
        indices.reserve(num_triangles * 3);

        // Read the triangle data
        for (uint32_t i = 0; i < num_triangles; ++i) {
            // Read the normal vector
            float normal[3];
            file.read((char*)normal, sizeof(normal));

            // Read the three vertices
            Vertex v1, v2, v3;
            file.read((char*)&v1.position.x, sizeof(v1.position.x));
            file.read((char*)&v1.position.y, sizeof(v1.position.y));
            file.read((char*)&v1.position.z, sizeof(v1.position.z));
            file.read((char*)&v2.position.x, sizeof(v2.position.x));
            file.read((char*)&v2.position.y, sizeof(v2.position.y));
            file.read((char*)&v2.position.z, sizeof(v2.position.z));
            file.read((char*)&v3.position.x, sizeof(v3.position.x));
            file.read((char*)&v3.position.y, sizeof(v3.position.y));
            file.read((char*)&v3.position.z, sizeof(v3.position.z));

            v1.normal = glm::vec3(normal[0], normal[1], normal[2]); //by convention we store the facet normal in the first vertex

            // Add the vertices to the vertex vector and update the indices
            unsigned int index = vertices.size();
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(index + 2);

            // Read the attribute byte count (not used)
            char attr_byte_count;
            file.read(&attr_byte_count, 1);
        }

        return true;
	}

    // Parse an STL file and extract the data
    bool ModelLoader::ParseSTL_ASCII(const std::string& file_path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
        // Open the file in text mode
        std::ifstream file(file_path);
        if (!file) {
            // Failed to open the file
            return false;
        }

        // Read the file line by line
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream line_stream(line);
            std::string keyword;
            line_stream >> keyword;

            if (keyword == "facet") {
                // Read the normal vector
                std::string normal_keyword;
                line_stream >> normal_keyword;
                float normal[3];
                line_stream >> normal[0] >> normal[1] >> normal[2];

                // Read the three vertices
                Vertex v1, v2, v3;
                std::string vertex_keyword;
                line_stream >> vertex_keyword;
                line_stream >> v1.position.x >> v1.position.y >> v1.position.z;
                line_stream >> vertex_keyword;
                line_stream >> v2.position.x >> v2.position.y >> v2.position.z;
                line_stream >> vertex_keyword;
                line_stream >> v3.position.x >> v3.position.y >> v3.position.z;

                // Set the normal and color for each vertex
                v1.normal = glm::vec3(normal[0], normal[1], normal[2]); //By convention, store the normal in the first vertex
            }
        }
    }
	

	// Parse a any file and extract the data
	bool ModelLoader::ParseFile(const std::string& file_path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
		FileType file_type = GetFileType(file_path);
		switch (file_type) {
		case FileType::OBJ:
			return ParseOBJ(file_path, vertices, indices);
		case FileType::STL:
			return ParseSTL(file_path, vertices, indices);
		default:
			// Unknown file type
			return false;
		}
	}

	ModelLoader::FileType ModelLoader::GetFileType(const std::string& file_path) {
		std::string extension = GetFileExtension(file_path);
		if (extension == "obj") {
			return FileType::OBJ;
		}
		else if (extension == "stl") {
			return FileType::STL;
		}
		else {
			// Unknown file type
			return FileType::UNKNOWN;
		}
	}

	std::string ModelLoader::GetFileExtension(const std::string& filepath) {
		size_t pos = filepath.find_last_of(".");
		if (pos == std::string::npos) {
			// No extension found
			return "";
		}
		return filepath.substr(pos + 1);
	}

    std::string ModelLoader::GetFileName(const std::string& filepath)
    {
        // Find the last occurrence of the directory separator character
        std::size_t pos = filepath.find_last_of("\\/");

        // If the separator was not found, return the entire filepath
        if (pos == std::string::npos) {
            return filepath;
        }

        // Return the part of the filepath after the separator
        return filepath.substr(pos + 1);
    }

}