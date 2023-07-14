#include "glpch.h"
#include "Mesh.h"
#include "Merlin/Memory/IndexBuffer.h"

namespace Merlin::Graphics {
	Mesh::Mesh(std::string name) : RenderableObject(name) {
		_drawMode = GL_POINTS;
		//Create VAO, VBO 
		_vao = CreateScope<VAO>();
		Console::trace("Mesh") << "Loaded " << _vertices.size() << " vertices." << Console::endl;
	}

	Mesh::Mesh(std::string name, std::vector<Vertex>& vertices, GLuint mode) : RenderableObject(name) {
		_drawMode = mode;
		//Move vertices data
		_vertices = vertices;

		//Create VAO, VBO
		_vao = CreateScope<VAO>();
		_vao->Bind();
		VBO vbo(_vertices);
		_vao->AddBuffer(vbo, Vertex::GetLayout());
		_vao->Unbind();

		Console::info("Mesh") << "Loaded " << vertices.size() << " vertices." << Console::endl;
	}

	Mesh::Mesh(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode) : RenderableObject(name) {
		_drawMode = mode;
		//Move vertices data
		_vertices = vertices;
		_indices = indices;

		//Create VAO, VBO
		_vao = CreateScope<VAO>();
		_vao->Bind();
		VBO vbo(_vertices);
		EBO ebo(_indices);
		_vao->AddBuffer(vbo, Vertex::GetLayout());
		_vao->Unbind();

		Console::info("Mesh") << "Loaded " << vertices.size() << " vertices." << Console::endl;
	}

	Shared<Mesh> Mesh::Create(std::string name) {
		return CreateShared<Mesh>(name);
	}

	Shared<Mesh> Mesh::Create(std::string name, std::vector<Vertex>& vertices, GLuint mode) {
		return CreateShared<Mesh>(name, vertices, mode);
	}

	Shared<Mesh> Mesh::Create(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode) {
		return CreateShared<Mesh>(name, vertices, indices, mode);
	}


	void Mesh::Bind() {
		_vao->Bind();
	}

	void Mesh::Unbind() {
		_vao->Unbind();
	}

	void Mesh::Draw() const {
		_vao->Bind();
		if (_indices.size() > 0) glDrawElements(_drawMode, _indices.size(), GL_UNSIGNED_INT, nullptr); //Draw elements using EBO
		else glDrawArrays(_drawMode, 0, _vertices.size()); //Draw
		_vao->Unbind();
	}

	void Mesh::DrawInstanced(GLsizeiptr instances) const {
		_vao->Bind();
		if (_indices.size() > 0) glDrawElementsInstanced(_drawMode, _indices.size(), GL_UNSIGNED_INT, nullptr, instances); //Draw elements using EBO
		else glDrawArraysInstanced(_drawMode, 0, _vertices.size(), instances); //Draw
		_vao->Unbind();
	}

	void Mesh::RecalculateNormals(){
		// Initialize all normals to zero
		for (auto& vertex : _vertices) {
			vertex.normal = glm::vec3(0);
		}

		// Calculate the face normal for each triangle and accumulate the normals for each vertex
		Console::info("Mesh") << "Recomputing Mesh normals.." << Console::endl;
		for (size_t i = 0; i < _indices.size(); i += 3) {
			GLuint i0 = _indices[i];
			GLuint i1 = _indices[i + 1];
			GLuint i2 = _indices[i + 2];

			glm::vec3 v0 = _vertices[i0].position;
			glm::vec3 v1 = _vertices[i1].position;
			glm::vec3 v2 = _vertices[i2].position;

			glm::vec3 edge1 = v1 - v0;
			glm::vec3 edge2 = v2 - v0;

			glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

			_vertices[i0].normal += faceNormal;
			_vertices[i1].normal += faceNormal;
			_vertices[i2].normal += faceNormal;
			Console::printProgress(float(i) / float(_indices.size()));
		}
		Console::print() << Console::endl;

		// Normalize the accumulated normals
		for (auto& vertex : _vertices) {
			vertex.normal = glm::normalize(vertex.normal);
		}

		//Update VAO, VBO
		_vao.reset();
		_vao = CreateScope<VAO>();
		_vao->Bind();
		VBO vbo(_vertices);
		EBO ebo(_indices);
		_vao->AddBuffer(vbo, Vertex::GetLayout());
		_vao->Unbind();
	}

	void Mesh::RecalculateIndices(){

		Console::info("Mesh") << "Recomputing Mesh indices.." << Console::endl;
		int i = 0;
		for (Vertex v : _vertices) {
			int j = 0;
			for (Vertex c : _vertices) {
				if (v.position == c.position) {
					_indices[j] = i;
				}
				j++;
			}
			i++;
			Console::printProgress(float(i) / float(_indices.size()));
		}
		Console::print() << Console::endl;

	}

	void Mesh::UpdateVAO() {
		//Update VAO, VBO
		_vao.reset();
		_vao = CreateScope<VAO>();
		_vao->Bind();
		VBO vbo(_vertices);
		EBO ebo(_indices);
		_vao->AddBuffer(vbo, Vertex::GetLayout());
		_vao->Unbind();
	}


	void Mesh::RemoveUnusedVertices() {

		Console::info("Mesh") << "Removing unused vertices.." << Console::endl;
		int i = 0;
		Vertices newVertices;

		newVertices.reserve(*std::max_element(_indices.begin(), _indices.end()));

		for (size_t i = 0; i < _indices.size(); i++) {
			newVertices.push_back(_vertices[i]);
		}
	}

	
}