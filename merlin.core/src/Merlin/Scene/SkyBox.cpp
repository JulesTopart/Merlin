#include "glpch.h"
#include "SkyBox.h"
#include "Merlin/Memory/VertexBuffer.h"
#include "Merlin/Memory/IndexBuffer.h"

namespace Merlin::Graphics {

	Shared<SkyBox> SkyBox::Create(std::string name, std::vector<std::string> paths) {
		return std::make_shared<SkyBox>(name, paths);
	}


	SkyBox::SkyBox(std::string name) : RenderableObject(name){
		std::vector<float> skyboxVertices =
		{
			//   Coordinates
			-1.0f, -1.0f,  1.0f,//        7--------6
			 1.0f, -1.0f,  1.0f,//       /|       /|
			 1.0f, -1.0f, -1.0f,//      4--------5 |
			-1.0f, -1.0f, -1.0f,//      | |      | |
			-1.0f,  1.0f,  1.0f,//      | 3------|-2
			 1.0f,  1.0f,  1.0f,//      |/       |/
			 1.0f,  1.0f, -1.0f,//      0--------1
			-1.0f,  1.0f, -1.0f
		};

		std::vector<GLuint> skyboxIndices =
		{
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

		//Create VAO, VBO
		_vao = CreateScope<VAO>();
		_vao->Bind();
		VBO vbo(skyboxVertices);
		EBO ebo(skyboxIndices);

		VertexBufferLayout layout;
		layout.Push<float>(3); //Vertex pos

		_vao->AddBuffer(vbo,layout);
		_vao->Unbind();

	}

	SkyBox::SkyBox(std::string name, std::vector<std::string> paths) : RenderableObject(name) {
		std::vector<float> skyboxVertices =
		{
			//   Coordinates
			-1.0f,  1.0f, -1.0f,//        7--------6
			 1.0f,  1.0f, -1.0f,//       /|       /|
			 1.0f, -1.0f, -1.0f,//      4--------5 |
			-1.0f, -1.0f, -1.0f,//      | |      | |
			-1.0f,  1.0f,  1.0f,//      | 3------|-2
			 1.0f,  1.0f,  1.0f,//      |/       |/
			 1.0f, -1.0f,  1.0f,//      0--------1
			-1.0f, -1.0f,  1.0f
		};

		std::vector<GLuint> skyboxIndices =
		{
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

		//Create VAO, VBO
		_vao = CreateScope<VAO>();
		_vao->Bind();
		VBO vbo(skyboxVertices);
		EBO ebo(skyboxIndices);

		VertexBufferLayout layout;
		layout.Push<float>(3); //Vertex pos

		_vao->AddBuffer(vbo, layout);
		_vao->Unbind();

		LoadCubeMap(paths);

	}

	void SkyBox::Draw() const {

		if (_cubeMap)
			_cubeMap->Bind(0);
		else Console::error("SkyBox") << "The cubemap is not initialized, Please use skybox.LoadCubeMap(...) of provide an existing cubemap" << Console::endl;

		// Render the skybox cube
		_vao->Bind();
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		_vao->Unbind();
		_cubeMap->Unbind();

	}

	void SkyBox::LoadCubeMap(std::vector<std::string> paths){
		_cubeMap = CreateShared<CubeMap>(paths);
	}


}