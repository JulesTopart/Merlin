#include "glpch.h"
#include "screenQuadrenderer.h"
#include "merlin/graphics/ressourceManager.h"

namespace Merlin {
	ScreenQuadRenderer::ScreenQuadRenderer() : m_shader(nullptr) {
		m_shader = ShaderLibrary::instance().get("screen.space");
		if (!m_shader) {
			Console::error("ScreenQuadRenderer") << "ShaderLibrary failed to retrieve screen space shader" << Console::endl;
		}
	}

	void ScreenQuadRenderer::render() {
		if (!m_shader) return;
		m_shader->use(); //Activate shader
		m_vao.bind(); //bind empty geometry
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 6); //draw a screen squad
		glEnable(GL_DEPTH_TEST);
	}

	void ScreenQuadRenderer::render(const Shared<TextureBase>& tex) {
		if (!m_shader) return;
		tex->bind(); //bind texture
		m_shader->use(); //Activate shader

		if (tex->type() == TextureType::DEPTH || tex->type() == TextureType::SHADOW)
			m_shader->setInt("mode", 1); //Activate shader
		
		tex->syncTextureUnit(*m_shader, "screen");
		m_vao.bind(); //bind empty geometry
		
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 6); //draw a screen squad
		tex->unbind(); //Unind Texture
		m_vao.unbind(); //Unind geometry
		glEnable(GL_DEPTH_TEST);
	}

}