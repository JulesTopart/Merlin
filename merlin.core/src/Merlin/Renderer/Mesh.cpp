#include "glpch.h"
#include "Mesh.h"

#include "glm/gtc/type_ptr.hpp"
#include "IndexBuffer.h"

namespace Merlin::Renderer {
	Mesh::Mesh(std::string n) {
		_name = n;
		model = glm::mat4(1.0f); //Set to identity
	}

	Mesh::~Mesh() {}

	void Mesh::Draw(std::shared_ptr<Shader> shader, glm::mat4 view) {
		shader->Use();

		unsigned int numDiffuse = 0;
		unsigned int numSpecular = 0;

		for (size_t i = 0; i < textures.size(); i++) {
			std::string num;
			if (textures[i]->type() == Texture::Type::DIFFUSE) {
				num = std::to_string(numDiffuse++);
			}
			else if (textures[i]->type() == Texture::Type::SPECULAR) {
				num = std::to_string(numSpecular++);
			}
			else {
				Console::error() << "Invalid texture type" << Console::endl;
			}
			textures[i]->SetUnit(i);
			textures[i]->SyncTextureUnit(shader, (textures[i]->typeToString() + num));
			textures[i]->Bind();
		}

		shader->SetMatrix4f("view", view); //Sync camera with GPU
		shader->SetMatrix4f("model", model); //Sync camera with GPU

		vao.Bind();
		if (indices.size() > 0) glDrawElements(drawMode, indices.size(), GL_UNSIGNED_INT, nullptr); //Draw elements using EBO
		else glDrawArrays(drawMode, 0, vertices.size()); //Draw
		vao.Unbind();
	}

	Mesh& Mesh::LoadTexture(std::string path, Texture::Type t, GLuint format) {

		std::shared_ptr<Texture> tex = std::make_shared<Texture>();
		tex->LoadFromFile(path, t, format);
		textures.push_back(tex);

		return *this;
	}

	Mesh& Mesh::LoadTexture(Shared<Texture> tex) {
		textures.push_back(tex);
		return *this;
	}

	Mesh& Mesh::LoadVertex(std::vector<Vertex>& _vertices) {

		vertices = std::move(_vertices);
		_vertices.clear();

		Console::info("Mesh") << "Loaded " << vertices.size() << " verticles." << Console::endl;
		
		vao.Bind();
		VBO vbo(vertices);

		vao.AddBuffer(vbo, Vertex::GetLayout());
		vao.Unbind();

		return *this;
	}

	Mesh& Mesh::LoadVertex(std::vector<Vertex>& _vertices, std::vector<GLuint>& _indices) {

		vertices = std::move(_vertices);
		_vertices.clear();
		indices = std::move(_indices);
		_indices.clear();

		Console::info("Mesh") << "Loaded " << vertices.size() << " vertices." << Console::endl;
		Console::info("Mesh") << "Loaded " << indices.size() << " facets." << Console::endl;

		vao.Bind();
		VBO vbo(vertices);
		EBO ebo(indices);

		vao.AddBuffer(vbo, Vertex::GetLayout());

		vao.Unbind();
		ebo.Unbind();

		return *this;
	}


	Mesh& Mesh::LinkShader(std::string shaderName, std::shared_ptr<Shader>& shader) {
		linkedShaderName = shaderName;
		_shader = shader;
		return *this;
	}

	std::shared_ptr<Shader> Mesh::GetLinkedShader() const {
		return _shader;
	}

	std::string Mesh::GetLinkedShaderName() const {
		return linkedShaderName;
	}

	Mesh& Mesh::SetDrawMode(GLuint mode) {
		drawMode = mode;
		return *this;
	}

	Mesh& Mesh::translate(glm::vec3 v) {
		model = glm::translate(model, v);
		return *this;
	}

	Mesh& Mesh::rotate(glm::vec3 v) {
		model = glm::rotate(model, v.x, glm::vec3(1, 0, 0));
		model = glm::rotate(model, v.y, glm::vec3(0, 1, 0));
		model = glm::rotate(model, v.z, glm::vec3(0, 0, 1));
		return *this;
	}

	Mesh& Mesh::rotate(float angle, glm::vec3 v) {
		model = glm::rotate(model, angle, v);
		return *this;
	}
}