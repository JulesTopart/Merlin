#include "glpch.h"
#include "ScreenQuadRenderer.h"

namespace Merlin {
	ScreenQuadRenderer::ScreenQuadRenderer() : m_shader("screen"){

		m_shader.compile(
			"assets/common/shaders/screen.space.vert",
			"assets/common/shaders/screen.space.frag"
		);

		m_shader.use();
		m_shader.setInt("screen", 0); //Link render texture to the render program
	}

	void ScreenQuadRenderer::render() {
		m_shader.use(); //Activate shader
		m_vao.bind(); //bind empty geometry
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 6); //draw a screen squad
		glEnable(GL_DEPTH_TEST);
	}

	void ScreenQuadRenderer::render(const Shared<TextureBase>& tex) {
		tex->bind(); //bind texture
		m_shader.use(); //Activate shader
		m_shader.setInt("mode", tex->getFormat() == GL_DEPTH_COMPONENT); //Activate shader
		
		tex->syncTextureUnit(m_shader, "screen");
		m_vao.bind(); //bind empty geometry
		
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 6); //draw a screen squad
		tex->unbind(); //Unind Texture
		m_vao.unbind(); //Unind geometry
		glEnable(GL_DEPTH_TEST);
	}

}