#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/VertexArray.h"
#include "Merlin/Graphics/RenderableObject.h"

using namespace Merlin::Memory;

namespace Merlin::Graphics {

	class Mesh : public RenderableObject {
	public:
		Mesh();
		Mesh(std::vector<Vertex>& vertices, GLuint mode = GL_TRIANGLES);
		Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode = GL_TRIANGLES);

		void Bind();
		void Unbind();

		void Draw() override;
		void DrawInstanced(GLsizeiptr instanced) override;

		void RecalculateNormals();
		void RecalculateIndices();
		void RemoveUnusedVertices();
		void UpdateVAO();

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