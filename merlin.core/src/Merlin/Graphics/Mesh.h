#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/Shader.h"
#include "Merlin/Scene/Material.h"
#include "Merlin/Memory/VertexArray.h"
#include "Merlin/Graphics/RenderableObject.h"

using namespace Merlin::Memory;

namespace Merlin::Graphics {

	class Mesh : public RenderableObject {
	public:
		Mesh(std::string name);
		Mesh(std::string name, std::vector<Vertex>& vertices, GLuint mode = GL_TRIANGLES);
		Mesh(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode = GL_TRIANGLES);

		void Bind();
		void Unbind();

		void Draw() const;
		void DrawInstanced(GLsizeiptr instanced) const;

		void RecalculateNormals();
		void RecalculateIndices();
		void RemoveUnusedVertices();
		void UpdateVAO();

		inline void SetDrawMode(GLuint mode) { _drawMode = mode; }
		inline void SetShader(Shared<Shader> shader) { _shader = shader; }
		inline void SetMaterial(Shared<Material> material) { _material = material; }
		inline void SetShader(std::string shaderName) { _shaderName = shaderName; }
		inline void SetMaterial(std::string materialName) { _materialName = materialName; }

		inline bool HasIndices() const { return _indices.size() > 0; }
		inline bool HasShader() const { return _shader != nullptr; }
		inline bool HasMaterial() const { return _material != nullptr; }

		inline GLuint GetDrawMode() const { return _drawMode; }
		inline const std::vector<Vertex>& GetVertices() const { return _vertices;  }
		inline const std::vector<GLuint>& GetIndices() const{ return _indices; }
		inline const glm::mat4& GetTransform() const { return _transform; }
		inline const Shader& GetShader() const { return *_shader; }
		inline const std::string& GetShaderName() const { return _shaderName; }
		inline const Material& GetMaterial() const { return *_material; }
		inline const std::string& GetMaterialName() const { return _materialName; }

		static Shared<Mesh> Create(std::string name);
		static Shared<Mesh> Create(std::string name, std::vector<Vertex>& vertices, GLuint mode = GL_TRIANGLES);
		static Shared<Mesh> Create(std::string name, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode = GL_TRIANGLES);

	private:
		Scope<VAO> _vao;
		GLuint _drawMode;
		std::vector<Vertex> _vertices;
		std::vector<GLuint> _indices;

		std::string _materialName = "default";
		std::string _shaderName = "default";

		Shared<Material> _material;
		Shared<Shader> _shader;
	};
}