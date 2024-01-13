#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/Shader.h"
#include "Merlin/Graphics/Material.h"
#include "Merlin/Memory/VertexArray.h"
#include "Merlin/Graphics/RenderableObject.h"

using namespace Merlin::Memory;

namespace Merlin::Graphics {

	struct BoundingBox {
		glm::vec3 min, max;
	};

	class Mesh : public RenderableObject {
	public:
		Mesh(std::string name);
		Mesh(std::string name, std::vector<Vertex>& vertices, GLuint mode = GL_TRIANGLES);
		Mesh(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode = GL_TRIANGLES);

		void Bind();
		void Unbind();

		void Draw() const;
		void DrawInstanced(GLsizeiptr instanced) const;

		void CalculateBoundingBox();
		void CalculateNormals();
		void CalculateIndices();
		void RemoveUnusedVertices();
		void UpdateVAO();

		inline void SetDrawMode(GLuint mode) { m_drawMode = mode; }
		inline void SetShader(Shared<Shader> shader) { m_shader = shader; }
		inline void SetMaterial(Shared<Material> material) { m_material = material; }
		inline void SetShader(std::string shaderName) { m_shaderName = shaderName; }
		inline void SetMaterial(std::string materialName) { m_materialName = materialName; }

		inline bool HasIndices() const { return m_indices.size() > 0; }
		inline bool HasShader() const { return m_shader != nullptr; }
		inline bool HasMaterial() const { return m_material != nullptr; }

		inline GLuint GetDrawMode() const { return m_drawMode; }
		inline const std::vector<Vertex>& GetVertices() const { return m_vertices;  }
		inline const std::vector<GLuint>& GetIndices() const{ return m_indices; }
		inline const glm::mat4& GetTransform() const { return m_transform; }
		inline const Shader& GetShader() const { return *m_shader; }
		inline const std::string& GetShaderName() const { return m_shaderName; }
		inline const Material& GetMaterial() const { return *m_material; }
		inline const std::string& GetMaterialName() const { return m_materialName; }

		inline BoundingBox GetBoundingBox() const { return m_bbox; }

		static Shared<Mesh> Create(std::string name);
		static Shared<Mesh> Create(std::string name, std::vector<Vertex>& vertices, GLuint mode = GL_TRIANGLES);
		static Shared<Mesh> Create(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode = GL_TRIANGLES);

	private:
		VAO m_vao;
		GLuint m_drawMode;
		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;

		BoundingBox m_bbox;

		std::string m_materialName = "default";
		std::string m_shaderName = "default";

		Shared<Material> m_material;
		Shared<Shader> m_shader;

		bool debugnormal = true;
	};

	typedef Shared<Mesh> Mesh_Ptr;
}