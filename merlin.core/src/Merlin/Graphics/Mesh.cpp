#include "glpch.h"
#include "Mesh.h"
#include "Merlin/Memory/IndexBuffer.h"

namespace Merlin::Graphics {
	Mesh::Mesh() {
		_drawMode = GL_POINTS;
		//Create VAO, VBO 
		_vao = CreateScope<VAO>();
		Console::trace("Mesh") << "Loaded " << _vertices.size() << " vertices." << Console::endl;
	}

	Mesh::Mesh(std::vector<Vertex>& vertices, GLuint mode) {
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

	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode) {
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

	void Mesh::Bind() {
		_vao->Bind();
	}

	void Mesh::Unbind() {
		_vao->Unbind();
	}

	void Mesh::Draw() {
		_vao->Bind();
		if (_indices.size() > 0) glDrawElements(_drawMode, _indices.size(), GL_UNSIGNED_INT, nullptr); //Draw elements using EBO
		else glDrawArrays(_drawMode, 0, _vertices.size()); //Draw
		_vao->Unbind();
	}

	void Mesh::DrawInstanced(GLsizeiptr instances) {
		_vao->Bind();
		if (_indices.size() > 0) glDrawElementsInstanced(_drawMode, _indices.size(), GL_UNSIGNED_INT, nullptr, instances); //Draw elements using EBO
		else glDrawArraysInstanced(_drawMode, 0, _vertices.size(), instances); //Draw
		_vao->Unbind();
	}

	
}