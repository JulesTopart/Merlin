#pragma once
#include <glad/glad.h>
#include "Shader.h"
#include "Texture.h"
#include "../Util/Camera.h"
#include "VertexArray.h"

namespace Merlin::Renderer {

	class Mesh {
	public:
		Mesh(std::string name);
		~Mesh();

		void Draw(Shader&, Utils::Camera&); //Draw the mesh


		Mesh& LinkShader(std::string shaderName, Shader& shader);
		std::string GetLinkedShaderName() const;
		Shader& GetLinkedShader() const;

		Mesh& LoadTexture(std::string path, Texture::Type t = Texture::Type::DIFFUSE, GLuint format = GL_RGBA);
		Mesh& LoadVertex(std::vector<Vertex>& _vertices);
		Mesh& LoadVertex(std::vector<Vertex>& _vertices, std::vector<GLuint>& _indices);

		inline glm::mat4& GetModelMatrix() { return model; }

		Mesh& translate(glm::vec3);
		Mesh& rotate(glm::vec3);
		Mesh& rotate(float angle, glm::vec3 v);
		Mesh& SetDrawMode(GLuint mode);

		inline const std::string name() const { return _name; }

	private:
		GLuint vertices;
		GLuint indices;

		std::vector<Texture> textures;

		std::string _name;

		VertexArray  vao;
		glm::mat4 model;

		GLuint drawMode = GL_TRIANGLE_STRIP;
		std::string linkedShaderName = "default";

		Shader* _shader = nullptr;
	};

}