#pragma once

#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "Merlin/Core/Core.h"
#include "VertexArray.h"
#include "Shader.h"

namespace Merlin::Renderer {

	class Primitive {
	public:
		Primitive();
		Primitive(std::vector<Vertex>& vertices, GLuint mode = GL_TRIANGLES);
		Primitive(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode = GL_TRIANGLES);

		void Bind();
		void Unbind();

		void Draw(Shared<Shader> shader, glm::mat4 view);
		void DrawInstanced(Shared<Shader> shader, glm::mat4 view, GLsizeiptr instanced);

		//Transformation
		void Translate(glm::vec3);
		void Rotate(glm::vec3);
		void Rotate(float angle, glm::vec3 v);

		inline void SetDrawMode(GLuint mode) { _drawMode = mode; }
		inline GLuint GetDrawMode() const { return _drawMode; }
		inline glm::mat4& GetModelMatrix() { return _model; }


		inline bool HasIndices() const { return _indices.size() > 0; }
		inline std::vector<Vertex>& GetVertices() { return _vertices;  }
		inline std::vector<GLuint>& GetIndices() { return _indices; }

		//Utils
		static Shared<Primitive> CreateRectangle(float x, float y);
		static Shared<Primitive> CreatePoint();
		static Shared<Primitive> CreateLine(float length, glm::vec3 dir = glm::vec3(1,0,0));
		static Shared<Primitive> CreateCoordSystem();
		static Shared<Primitive> CreateCube(float w);
		static Shared<Primitive> CreateCube(float x, float y, float z);
		static Shared<Primitive> CreateSphere(float r, int hres = 10, int vres = 10);

	private:
		Scope<VAO> _vao;
		std::vector<Vertex> _vertices;
		std::vector<GLuint> _indices;

		//Transformation
		glm::mat4 _model;

		//DrawMode (Line, Triangles, points...)
		GLuint _drawMode;
	};
}