#pragma once
#include "Merlin/Core/Core.h"

#include <vector>
#include "Merlin/Memory/VertexArray.h"

using namespace Merlin::Memory;

namespace Merlin::Graphics {

	class Mesh {
	public:
		Mesh();
		Mesh(std::vector<Vertex>& vertices, GLuint mode = GL_TRIANGLES);
		Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode = GL_TRIANGLES);

		void Bind();
		void Unbind();

		void Draw();
		void DrawInstanced(GLsizeiptr instanced);

		inline void SetDrawMode(GLuint mode) { _drawMode = mode; }
		inline GLuint GetDrawMode() const { return _drawMode; }

		inline bool HasIndices() const { return _indices.size() > 0; }
		inline std::vector<Vertex>& GetVertices() { return _vertices;  }
		inline std::vector<GLuint>& GetIndices() { return _indices; }



	private:
		Scope<VAO> _vao;
		std::vector<Vertex> _vertices;
		std::vector<GLuint> _indices;

		//DrawMode (Line, Triangles, points...)
		GLuint _drawMode;
	};
}