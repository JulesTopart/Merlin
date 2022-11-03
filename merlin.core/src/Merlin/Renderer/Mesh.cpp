#include "glpch.h"
#include "Mesh.h"

#include "glm/gtc/type_ptr.hpp"
#include "IndexBuffer.h"

using Camera = Merlin::Utils::Camera;

namespace Merlin::Renderer {
	Mesh::Mesh(std::string n) {
		_name = n;
		model = glm::mat4(1.0f); //Set to identity
	}

	Mesh::~Mesh() {
		for (Texture& t : textures) {
			t.Delete();
		}
	}

	void Mesh::Draw(Shader& shader, Camera& camera) {
		shader.Use();

		unsigned int numDiffuse = 0;
		unsigned int numSpecular = 0;

		for (size_t i = 0; i < textures.size(); i++) {
			std::string num;
			if (textures[i].type() == Texture::Type::DIFFUSE) {
				num = std::to_string(numDiffuse++);
			}
			else if (textures[i].type() == Texture::Type::SPECULAR) {
				num = std::to_string(numSpecular++);
			}
			else {
				Console::error() << "Invalid texture type" << Console::endl;
			}
			textures[i].textureUnit(shader, (textures[i].typeToString() + num), i);
			textures[i].Bind();
		}

		shader.SetMatrix4f("view", camera.GetViewMatrix()); //Sync camera with GPU

		vao.Bind();
		if (indices.size() > 0) glDrawElements(drawMode, indices.size(), GL_UNSIGNED_INT, nullptr); //Draw elements using EBO
		else glDrawArrays(drawMode, 0, vertices.size()); //Draw
		vao.Unbind();
	}

	Mesh& Mesh::LoadTexture(std::string path, Texture::Type t, GLuint format) {

		Texture tex;
		tex.Create();
		tex.Load(path, t, format);
		textures.push_back(tex);

		return *this;
	}

	Mesh& Mesh::LoadVertex(std::vector<Vertex>& _vertices) {

		vertices = _vertices;

		Console::info("Mesh") << "Loaded " << _vertices.size() << "verticles." << Console::endl;
		
		vao.Bind();
		VBO vbo(vertices);

		VertexBufferLayout layout;
		layout.Push<float>(3); //Vertex pos
		layout.Push<float>(3); //Vertex normal
		layout.Push<float>(3); //Vertex color
		layout.Push<float>(2); //Texture coordinates
		vao.AddBuffer(vbo, layout);
		vao.Unbind();

		return *this;
	}

	Mesh& Mesh::LoadVertex(std::vector<Vertex>& _vertices, std::vector<GLuint>& _indices) {

		vertices = _vertices;
		indices = _indices;

		Console::info("Mesh") << "Loaded " << _indices.size() << "facets." << Console::endl;

		vao.Bind();
		VBO vbo(vertices);
		EBO ebo(indices);

		VertexBufferLayout layout;
		layout.Push<float>(3); //Vertex pos
		layout.Push<float>(3); //Vertex normal
		layout.Push<float>(3); //Vertex color
		layout.Push<float>(2); //Texture coordinates
		vao.AddBuffer(vbo, layout);

		vao.Unbind();
		ebo.Unbind();

		return *this;
	}


	Mesh& Mesh::LinkShader(std::string shaderName, Shader& shader) {
		linkedShaderName = shaderName;
		_shader = &shader;
		return *this;
	}

	Shader& Mesh::GetLinkedShader() const {
		return *_shader;
	}

	std::string Mesh::GetLinkedShaderName() const {
		return linkedShaderName;
	}

	Mesh& Mesh::SetDrawMode(GLuint mode) {
		drawMode = mode;
		return *this;
	}

	Mesh& Mesh::translate(glm::vec3 v) {
		glm::translate(model, v);
		return *this;
	}

	Mesh& Mesh::rotate(glm::vec3 v) {
		glm::rotate(model, v.x, glm::vec3(1, 0, 0));
		glm::rotate(model, v.y, glm::vec3(0, 1, 0));
		glm::rotate(model, v.z, glm::vec3(0, 0, 1));
		return *this;
	}

	Mesh& Mesh::rotate(float angle, glm::vec3 v) {
		glm::rotate(model, angle, v);
		return *this;
	}
}