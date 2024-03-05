#include "glpch.h"
#include "SkyBox.h"
#include "Merlin/Memory/VertexBuffer.h"
#include "Merlin/Memory/IndexBuffer.h"

namespace Merlin {

	Shared<SkyBox> SkyBox::create(std::string name, std::vector<std::string> paths) {
		return std::make_shared<SkyBox>(name, paths);
	}
	Shared<SkyBox> SkyBox::create(std::string name) {
		return std::make_shared<SkyBox>(name);
	}

	SkyBox::SkyBox(std::string name) : RenderableObject(name, ObjectType::SKYBOX), m_vao(){
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

		//create VAO, VBO
		m_vao.bind(); //bind the VAO
		VBO vbo(skyboxVertices);
		EBO ebo(skyboxIndices);

		VertexBufferLayout layout;
		layout.push<float>(3); //Vertex pos
		m_vao.addBuffer(vbo, layout);
		m_vao.bindBuffer(ebo);
		m_vao.unbind();

	}

	SkyBox::SkyBox(std::string name, std::vector<std::string> paths) : RenderableObject(name, ObjectType::SKYBOX), m_vao() {
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

		//create VAO, VBO
		VBO vbo(skyboxVertices);
		EBO ebo(skyboxIndices);
		
		VertexBufferLayout layout;
		layout.push<float>(3); //Vertex pos
		m_vao.addBuffer(vbo, layout);
		m_vao.bindBuffer(ebo);
		m_vao.unbind();

		loadCubeMap(paths);

	}

	void SkyBox::draw() const {

		if (m_cubeMap)
			m_cubeMap->bind(0);
		else {
			//Console::trace("SkyBox") << "The cubemap is not initialized, Please use skybox.loadCubeMap(...) of provide an existing cubemap" << Console::endl;
			//return;
		}

		// Render the skybox cube
		m_vao.bind();
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		m_vao.unbind();

		//if (m_cubeMap);
			//m_cubeMap->unbind();

	}

	void SkyBox::loadCubeMap(std::vector<std::string> paths){
		m_cubeMap = createShared<CubeMap>(paths);
	}


}