#include "glpch.h"
#include "Primitive.h"
#include "IndexBuffer.h"

namespace Merlin::Renderer {

	/*
	Primitive::Primitive(Primitive& cpy) {
		if (cpy.HasIndices()) {
			_drawMode = cpy._drawMode;

			//Move vertices data
			_vertices = cpy._vertices;
			_indices = cpy._indices;

			_model = cpy._model;
			//Create VAO, VBO
			_vao = CreateScope<VAO>();
			_vao->Bind();
			VBO vbo(_vertices);
			EBO ebo(_indices);
			_vao->AddBuffer(vbo, Vertex::GetLayout());
			_vao->Unbind();
		}else {
			//Move vertices data
			_vertices = cpy._vertices;
			_model = cpy._model;
			//Create VAO, VBO
			_vao = CreateScope<VAO>();
			_vao->Bind();
			VBO vbo(_vertices);
			_vao->AddBuffer(vbo, Vertex::GetLayout());
			_vao->Unbind();
		}
	}*/


	Primitive::Primitive() {
		_drawMode = GL_POINTS;
		_model = glm::mat4(1.0f);
		//Create VAO, VBO 
		_vao = CreateScope<VAO>();
		Console::trace("Primitive") << "Loaded " << _vertices.size() << " vertices." << Console::endl;
	}

	Primitive::Primitive(std::vector<Vertex>& vertices, GLuint mode) {
		_drawMode = mode;
		_model = glm::mat4(1.0f);
		//Move vertices data
		_vertices = std::move(vertices);
		vertices.clear();

		//Create VAO, VBO
		_vao = CreateScope<VAO>();
		_vao->Bind();
		VBO vbo(_vertices);
		_vao->AddBuffer(vbo, Vertex::GetLayout());
		_vao->Unbind();

		Console::info("Primitive") << "Loaded " << vertices.size() << " vertices." << Console::endl;
	}

	Primitive::Primitive(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, GLuint mode) {
		_drawMode = mode;
		_model = glm::mat4(1.0f);
		//Move vertices data
		_vertices = std::move(vertices);
		vertices.clear();
		_indices = std::move(indices);
		indices.clear();

		//Create VAO, VBO
		_vao = CreateScope<VAO>();
		_vao->Bind();
		VBO vbo(_vertices);
		EBO ebo(_indices);
		_vao->AddBuffer(vbo, Vertex::GetLayout());
		_vao->Unbind();

		Console::info("Primitive") << "Loaded " << vertices.size() << " vertices." << Console::endl;
	}

	void Primitive::Bind() {
		_vao->Bind();
	}

	void Primitive::Unbind() {
		_vao->Unbind();
	}



	void Primitive::Translate(glm::vec3 v) {
		_model = glm::translate(_model, v);
	}

	void Primitive::Rotate(glm::vec3 v) {
		_model = glm::rotate(_model, v.x, glm::vec3(1, 0, 0));
		_model = glm::rotate(_model, v.y, glm::vec3(0, 1, 0));
		_model = glm::rotate(_model, v.z, glm::vec3(0, 0, 1));
	}

	void Primitive::Rotate(float angle, glm::vec3 v) {
		_model = glm::rotate(_model, angle, v);
	}

	void Primitive::Draw(Shared<Shader> shader, glm::mat4 view){
		shader->Use();
		shader->SetMatrix4f("view", view); //Sync camera with GPU
		shader->SetMatrix4f("model", _model); //Sync camera with GPU

		_vao->Bind();
		if (_indices.size() > 0) glDrawElements(_drawMode, _indices.size(), GL_UNSIGNED_INT, nullptr); //Draw elements using EBO
		else glDrawArrays(_drawMode, 0, _vertices.size()); //Draw
		_vao->Unbind();
	}

	void Primitive::DrawInstanced(Shared<Shader> shader, glm::mat4 view, GLsizeiptr instances) {
		shader->Use();
		shader->SetMatrix4f("view", view); //Sync camera with GPU
		shader->SetMatrix4f("model", _model); //Sync camera with GPU

		_vao->Bind();
		if (_indices.size() > 0) glDrawElementsInstanced(_drawMode, _indices.size(), GL_UNSIGNED_INT, nullptr, instances); //Draw elements using EBO
		else glDrawArraysInstanced(_drawMode, 0, _vertices.size(), instances); //Draw
		_vao->Unbind();
	}

	Shared<Primitive> Primitive::CreateRectangle(float x, float y) {
		Vertices v = {
			Vertex{glm::vec3(-x / 2.0f,-y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
			Vertex{glm::vec3( x / 2.0f,-y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
			Vertex{glm::vec3(-x / 2.0f, y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
			Vertex{glm::vec3( x / 2.0f,-y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
			Vertex{glm::vec3(-x / 2.0f, y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
			Vertex{glm::vec3( x / 2.0f, y / 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)}
		};

		Indices i = { 0, 1, 2, 3, 4, 5 };
		return CreateShared<Primitive>(v, i);
	}

	Shared<Primitive> Primitive::CreatePoint() {
		Vertices v = {
			Vertex{glm::vec3(0,0,0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)}
		};

		return CreateShared<Primitive>(v, GL_POINTS);
	}

	Shared<Primitive> Primitive::CreateLine(float length, glm::vec3 dir) {
		Vertices v = {
			Vertex{glm::vec3(0,0,0)},
			Vertex{glm::normalize(dir) * length}
		};

		return CreateShared<Primitive>(v, GL_LINES);
	}

	Shared<Primitive> Primitive::CreateCoordSystem() {
		Vertices v = {
			Vertex{ glm::vec3(0.2f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
			Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
			Vertex{ glm::vec3(0.0f, 0.2f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
			Vertex{ glm::vec3(0.0f, 0.0f, 0.2f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
			Vertex{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)}
		};

		return CreateShared<Primitive>(v, GL_LINES);
	}

	Shared<Primitive> Primitive::CreateCube(float w) { return CreateCube(w, w, w); }

	Shared<Primitive> Primitive::CreateCube(float x, float y, float z){

		float x_lo = -x / 2.0f;
		float x_up = x / 2.0f;
		float y_lo = -y / 2.0f;
		float y_up = y / 2.0f;
		float z_lo = -z / 2.0f;
		float z_up = z / 2.0f;

		std::vector<Vertex> v = {
			//   Coordinates
			 Vertex{ glm::vec3( x_lo, y_lo, z_up)},  //        7--------6
			 Vertex{ glm::vec3( x_up, y_lo, z_up)},  //       /|       /|
			 Vertex{ glm::vec3( x_up, y_lo, z_lo)},  //      4--------5 |
			 Vertex{ glm::vec3( x_lo, y_lo, z_lo)},  //      | |      | |
			 Vertex{ glm::vec3( x_lo, y_up, z_up)},  //      | 3------|-2
			 Vertex{ glm::vec3( x_up, y_up, z_up)},  //      |/       |/
			 Vertex{ glm::vec3( x_up, y_up, z_lo)},  //      0--------1
			 Vertex{ glm::vec3( x_lo, y_up, z_lo)}
		};

		std::vector<GLuint> i = {
			// Right
			1, 2, 6,
			6, 5, 1,
			// Left
			0, 4, 7,
			7, 3, 0,
			// Top
			4, 5, 6,
			6, 7, 4,
			// Bottom
			0, 3, 2,
			2, 1, 0,
			// Back
			0, 1, 5,
			5, 4, 0,
			// Front
			3, 7, 6,
			6, 2, 3
		};

		return CreateShared<Primitive>(v, i);
	}

	Shared<Primitive> Primitive::CreateSphere(float r, int hres, int vres){

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;

		vertices.resize(hres * vres * 6);
		normals.resize(hres * vres * 6);

		float d_h = 2 * glm::pi<float>() / ((float)hres);
		float d_v = glm::pi<float>() / ((float)vres);
		int n = 0;
		// Vertices are created inside this loop.
		for (int i = 0; i < hres; i++) {
			float h = i * d_h;
			float hn = h + d_h;
			for (int j = 0; j < vres; j++) {
				float v = j * d_v;
				float vn = v + d_v;

				// The sphere is consists of multiple triangles where 2 triangles make a plane.
				// These 4 points are the corners of said plane. To create a triangle 3 of these corners are
				// used counterclockwise with the 2nd triangle's first point being the 1st last point.
				// Normal vectors are the same as the points without the radius multiplied.
				glm::vec4 p0(glm::cos(h) * glm::sin(v), glm::sin(h) * glm::sin(v),
					glm::cos(v), 1.0f);
				glm::vec4 p1(glm::cos(h) * glm::sin(vn), glm::sin(h) * glm::sin(vn),
					glm::cos(vn), 1.0f);
				glm::vec4 p2(glm::cos(hn) * glm::sin(v), glm::sin(hn) * glm::sin(v),
					glm::cos(v), 1.0f);
				glm::vec4 p3(glm::cos(hn) * glm::sin(vn), glm::sin(hn) * glm::sin(vn),
					glm::cos(vn), 1.0f);
				vertices[n] = p0 * r;
				normals.at(n++) = glm::vec3(p0);
				vertices.at(n) = p1 * r;
				normals.at(n++) = glm::vec3(p1);
				vertices.at(n) = p3 * r;
				normals.at(n++) = glm::vec3(p3);
				vertices.at(n) = p3 * r;
				normals.at(n++) = glm::vec3(p3);
				vertices.at(n) = p2 * r;
				normals.at(n++) = glm::vec3(p2);
				vertices.at(n) = p0 * r;
				normals.at(n++) = glm::vec3(p0);
			}
		}


		Vertices v;
		for (int i(0); i < vertices.size(); i++) {
			v.push_back({ vertices[i], normals[i], glm::vec3(1,1,1) });
		}
		return CreateShared<Primitive>(v);
	}

}