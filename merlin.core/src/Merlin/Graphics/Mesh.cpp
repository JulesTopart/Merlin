#include "glpch.h"
#include "Mesh.h"
#include "Merlin/Memory/IndexBuffer.h"

namespace Merlin {
	Mesh::Mesh(std::string name) : RenderableObject(name) {
		m_drawMode = GL_POINTS;
		//Create VAO, VBO 
		Console::trace("Mesh") << "Loaded " << m_vertices.size() << " vertices." << Console::endl;
	}

	Mesh::Mesh(std::string name, std::vector<Vertex>& vertices, GLuint mode) : RenderableObject(name) {
		m_drawMode = mode;
		//Move vertices data
		m_vertices = vertices;

		//Create VAO, VBO
		VBO vbo(m_vertices);
		m_vao.addBuffer(vbo, Vertex::GetLayout());
		m_vao.unbind();

		Console::info("Mesh") << "Loaded " << vertices.size() << " vertices." << Console::endl;
	}

	Mesh::Mesh(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode) : RenderableObject(name) {
		m_drawMode = mode;
		//Move vertices data
		m_vertices = vertices;
		m_indices = indices;

		//Create VAO, VBO
		VBO vbo(m_vertices);
		EBO ebo(m_indices);
		m_vao.bindBuffer(ebo);
		m_vao.addBuffer(vbo, Vertex::GetLayout());
		m_vao.unbind();

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
		m_vao.bind();
	}

	void Mesh::Unbind() {
		m_vao.unbind();
	}

	void Mesh::Draw() const {
		m_vao.bind();
		if (m_indices.size() > 0) glDrawElements(m_drawMode, m_indices.size(), GL_UNSIGNED_INT, 0); //Draw elements using EBO
		else glDrawArrays(m_drawMode, 0, m_vertices.size()); //Draw
		m_vao.unbind();
	}

	void Mesh::DrawInstanced(GLsizeiptr instances) const {
		m_vao.bind();
		if (m_indices.size() > 0) glDrawElementsInstanced(m_drawMode, m_indices.size(), GL_UNSIGNED_INT, nullptr, instances); //Draw elements using EBO
		else glDrawArraysInstanced(m_drawMode, 0, m_vertices.size(), instances); //Draw
		m_vao.unbind();
	}

	void Mesh::CalculateBoundingBox() {
		for (Vertex& v : m_vertices) {
			if (v.position.x > m_bbox.max.x) m_bbox.max.x = v.position.x;
			if (v.position.y > m_bbox.max.y) m_bbox.max.y = v.position.y;
			if (v.position.z > m_bbox.max.z) m_bbox.max.z = v.position.z;

			if (v.position.x < m_bbox.min.x) m_bbox.min.x = v.position.x;
			if (v.position.y < m_bbox.min.y) m_bbox.min.y = v.position.y;
			if (v.position.z < m_bbox.min.z) m_bbox.min.z = v.position.z;
		}
		Console::info("Mesh") << "Bounding box is " << m_bbox.max - m_bbox.min << " starting at " << m_bbox.min << " and ending at " << m_bbox.max << Console::endl;
	}

	void Mesh::CalculateNormals(){
		// Initialize all normals to zero
		for (auto& vertex : m_vertices) {
			vertex.normal = glm::vec3(0);
		}

		// Calculate the face normal for each triangle and accumulate the normals for each vertex
		Console::info("Mesh") << "Recomputing Mesh normals.." << Console::endl;
		for (size_t i = 0; i < m_indices.size(); i += 3) {
			GLuint i0 = m_indices[i];
			GLuint i1 = m_indices[i + 1];
			GLuint i2 = m_indices[i + 2];

			glm::vec3 v0 = m_vertices[i0].position;
			glm::vec3 v1 = m_vertices[i1].position;
			glm::vec3 v2 = m_vertices[i2].position;

			glm::vec3 edge1 = v1 - v0;
			glm::vec3 edge2 = v2 - v0;

			glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

			m_vertices[i0].normal += faceNormal;
			m_vertices[i1].normal += faceNormal;
			m_vertices[i2].normal += faceNormal;
			Console::printProgress(float(i) / float(m_indices.size()));
		}
		Console::print() << Console::endl;

		// Normalize the accumulated normals
		for (auto& vertex : m_vertices) {
			vertex.normal = glm::normalize(vertex.normal);
		}

		//Update VAO, VBO
		VBO vbo(m_vertices);
		EBO ebo(m_indices);
		m_vao.bindBuffer(ebo);
		m_vao.addBuffer(vbo, Vertex::GetLayout());
		m_vao.unbind();
	}

	void Mesh::CalculateIndices(){

		Console::info("Mesh") << "Recomputing Mesh indices.." << Console::endl;
		int i = 0;
		for (Vertex v : m_vertices) {
			int j = 0;
			for (Vertex c : m_vertices) {
				if (v.position == c.position) {
					m_indices[j] = i;
				}
				j++;
			}
			i++;
			Console::printProgress(float(i) / float(m_indices.size()));
		}
		Console::print() << Console::endl;

	}

	void Mesh::UpdateVAO() {
		//Update VAO, VBO
		VBO vbo(m_vertices);
		EBO ebo(m_indices);
		m_vao.bindBuffer(ebo);
		m_vao.addBuffer(vbo, Vertex::GetLayout());
		m_vao.unbind();
	}


	void Mesh::RemoveUnusedVertices() {

		Console::info("Mesh") << "Removing unused vertices.." << Console::endl;
		int i = 0;
		Vertices newVertices;

		newVertices.reserve(*std::max_element(m_indices.begin(), m_indices.end()));

		for (size_t i = 0; i < m_indices.size(); i++) {
			newVertices.push_back(m_vertices[i]);
		}
	}

	
}