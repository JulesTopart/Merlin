#include "glpch.h"
#include "mesh.h"
#include "merlin/memory/indexBuffer.h"

#include <unordered_map>
#include <vector>


// Helper function to compare glm::vec3 for hashing
bool operator==(const glm::vec3& a, const glm::vec3& b) {
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

// Hash function for glm::vec3
namespace std {
	template<> struct hash<glm::vec3> {
		size_t operator()(const glm::vec3& v) const {
			return ((hash<float>()(v.x)
				^ (hash<float>()(v.y) << 1)) >> 1)
				^ (hash<float>()(v.z) << 1);
		}
	};
}

namespace Merlin {
	Mesh::Mesh(std::string name) : RenderableObject(name) {
		m_drawMode = GL_POINTS;

		m_vertices.push_back({ glm::vec3(0,0,0) });

		m_vao.bind();
		VBO vbo(m_vertices);
		m_vao.addBuffer(vbo, Vertex::getLayout());
		m_vao.unbind();
		Console::trace("Mesh") << "Loaded " << m_vertices.size() << " vertices." << Console::endl;
	}

	Mesh::Mesh(std::string name, std::vector<Vertex>& vertices, GLuint mode) : RenderableObject(name) {
		m_drawMode = mode;
		//Move vertices data
		m_vertices = vertices;

		//create VAO, VBO
		m_vao.bind();
		VBO vbo(m_vertices);
		m_vao.addBuffer(vbo, Vertex::getLayout());
		m_vao.unbind();
		Console::info("Mesh") << "Loaded " << vertices.size() << " vertices." << Console::endl;
	}

	Mesh::Mesh(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode) : RenderableObject(name) {
		m_drawMode = mode;
		//Move vertices data
		m_vertices = vertices;
		m_indices = indices;

		//create VAO, VBO
		m_vao.bind();
		VBO vbo(m_vertices);
		EBO ebo(m_indices);
		m_vao.addBuffer(vbo, Vertex::getLayout());
		m_vao.bindBuffer(ebo);
		m_vao.unbind();
		Console::info("Mesh") << "Loaded " << vertices.size() << " vertices." << Console::endl;
	}

	Shared<Mesh> Mesh::create(std::string name) {
		return createShared<Mesh>(name);
	}

	Shared<Mesh> Mesh::create(std::string name, std::vector<Vertex>& vertices, GLuint mode) {
		return createShared<Mesh>(name, vertices, mode);
	}

	Shared<Mesh> Mesh::create(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode) {
		return createShared<Mesh>(name, vertices, indices, mode);
	}


	void Mesh::bind() {
		m_vao.bind();
	}

	void Mesh::unbind() {
		m_vao.unbind();
	}

	void Mesh::draw() const {
		m_vao.bind();
		if (m_indices.size() > 0) glDrawElements(m_drawMode, m_indices.size(), GL_UNSIGNED_INT, 0); //draw elements using EBO
		else glDrawArrays(m_drawMode, 0, m_vertices.size()); //draw
		m_vao.unbind();
	}

	void Mesh::drawInstanced(GLsizeiptr instances) const {
		m_vao.bind();
		if (m_indices.size() > 0) glDrawElementsInstanced(m_drawMode, m_indices.size(), GL_UNSIGNED_INT, nullptr, instances); //draw elements using EBO
		else glDrawArraysInstanced(m_drawMode, 0, m_vertices.size(), instances); //draw
		m_vao.unbind();
	}



	void Mesh::smoothNormals() {
		// Clear any existing normals in the vertex data
		for (auto& vertex : m_vertices) {
			vertex.normal = glm::vec3(0.0f);
		}

		// Use a map to accumulate normals based on vertex positions
		std::unordered_map<glm::vec3, glm::vec3, std::hash<glm::vec3>, std::equal_to<glm::vec3>> normalMap;

		// Accumulate face normals for each vertex
		for (size_t i = 0; i < m_indices.size(); i += 3) {
			GLuint i0 = m_indices[i];
			GLuint i1 = m_indices[i + 1];
			GLuint i2 = m_indices[i + 2];

			glm::vec3 v0 = m_vertices[i0].position;
			glm::vec3 v1 = m_vertices[i1].position;
			glm::vec3 v2 = m_vertices[i2].position;

			// Calculate the face normal
			glm::vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

			// Accumulate the face normal into each vertex normal based on position
			normalMap[v0] += faceNormal;
			normalMap[v1] += faceNormal;
			normalMap[v2] += faceNormal;
		}

		// Normalize the accumulated normals and assign them to the vertices
		for (auto& vertex : m_vertices) {
			vertex.normal = glm::normalize(normalMap[vertex.position]);
		}

		// Update the Vertex Array Object (VAO) with the new normals
		updateVAO();
	}


	void Mesh::calculateBoundingBox() {
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

	void Mesh::calculateNormals(){
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
		m_vao.addBuffer(vbo, Vertex::getLayout());
		m_vao.unbind();
	}

	void Mesh::calculateIndices(){

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

	void Mesh::updateVAO() {
		//Update VAO, VBO
		m_vao.bind();
		VBO vbo(m_vertices);
		EBO ebo(m_indices);
		m_vao.bindBuffer(ebo);
		m_vao.addBuffer(vbo, Vertex::getLayout());
		m_vao.unbind();
	}


	void Mesh::removeUnusedVertices() {

		Console::info("Mesh") << "Removing unused vertices.." << Console::endl;
		int i = 0;
		Vertices newVertices;

		newVertices.reserve(*std::max_element(m_indices.begin(), m_indices.end()));

		for (size_t i = 0; i < m_indices.size(); i++) {
			newVertices.push_back(m_vertices[i]);
		}
	}

	
}