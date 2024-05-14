#include "glpch.h"
#include "environment.h"
#include "merlin/memory/vertexBuffer.h"
#include "merlin/memory/indexBuffer.h"
#include "merlin/memory/frameBuffer.h"
#include "merlin/memory/renderBuffer.h"

namespace Merlin {

	void Environment::setupMesh() {
		std::vector<float> vertices =
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

		std::vector<GLuint> indices =
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

		VBO vbo(vertices);
		EBO ebo(indices);

		VertexBufferLayout layout;
		layout.push<float>(3); // Vertex position
		m_vao.addBuffer(vbo, layout);
		m_vao.bindBuffer(ebo);
		m_vao.unbind();
	}


	Environment::Environment(std::string name, GLuint resolution) : RenderableObject(name, ObjectType::ENVIRONMENT), m_vao(), m_resolution(resolution) {
		setupMesh(); // Setup mesh for rendering cubemap or procedural backdrops
	}

	void Environment::attach(Shader& shader) const{
		if (!shader.supportEnvironment())return;

		shader.setInt("environment.use_irradiance_tex", m_irradiance != nullptr && shader.supportTexture());
		if (m_irradiance && shader.supportTexture()) {
			m_irradiance->setUnit(TextureBase::getNextTextureUnit());
			m_irradiance->bind();
			m_irradiance->syncTextureUnit(shader, "environment.irradiance_tex");
		}
		else shader.setVec3("environment.irradiance", glm::vec3(1.0));

		shader.setInt("environment.use_specular_tex", m_specular_tex != nullptr && shader.supportTexture());
		if (m_specular_tex && shader.supportTexture()) {
			m_specular_tex->setUnit(TextureBase::getNextTextureUnit());
			m_specular_tex->bind();
			m_specular_tex->syncTextureUnit(shader, "material.specular_tex");

		}
		else shader.setVec3("material.specular_color", m_specular_color);

		shader.setFloat("material.shininess", m_shininess);
	}

	void Environment::draw() const {
		// Render the skybox cube
		m_vao.bind();
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		m_vao.unbind();

	}

	void Environment::loadHDR(const std::string& path){
		raw_hdri = Texture2D::create(path, TextureType::ENVIRONMENT);
		FrameBuffer fbo(m_resolution, m_resolution);

		fbo.bind();
		fbo.addDepthStencilAttachment(fbo.createRenderBufferAttachment(GL_DEPTH_COMPONENT24));
		skybox = CubeMap::create(m_resolution, m_resolution);


	}
}