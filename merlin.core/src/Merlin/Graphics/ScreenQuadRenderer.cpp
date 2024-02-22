#include "glpch.h"
#include "ScreenQuadRenderer.h"

namespace Merlin {
	ScreenQuadRenderer::ScreenQuadRenderer() : m_shader("screen"){

		m_shader.Compile(
			"assets/common/shaders/screen.space.vert",
			"assets/common/shaders/screen.space.frag"
		);

		m_shader.Use();
		m_shader.SetInt("screen", 0); //Link render texture to the render program
	}

	void ScreenQuadRenderer::Render() {
		m_shader.Use(); //Activate shader
		m_vao.Bind(); //Bind empty geometry
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 6); //Draw a screen squad
		glEnable(GL_DEPTH_TEST);
	}

	void ScreenQuadRenderer::Render(const Shared<TextureBase>& tex) {
		tex->Bind(); //Bind texture
		m_shader.Use(); //Activate shader
		m_shader.SetInt("mode", tex->GetFormat() == GL_DEPTH_COMPONENT); //Activate shader
		
		tex->SyncTextureUnit(m_shader, "screen");
		m_vao.Bind(); //Bind empty geometry
		
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLES, 0, 6); //Draw a screen squad
		tex->Unbind(); //Unind Texture
		m_vao.Unbind(); //Unind geometry
		glEnable(GL_DEPTH_TEST);
	}

}