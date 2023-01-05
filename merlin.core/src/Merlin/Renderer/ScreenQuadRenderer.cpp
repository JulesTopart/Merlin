#include "glpch.h"
#include "ScreenQuadRenderer.h"


namespace Merlin::Renderer {
	ScreenQuadRenderer::ScreenQuadRenderer() {
		
		vao = std::make_shared<VAO>();

		shader = std::make_shared<Shader>("screen");
		shader->Compile(
			"assets/shaders/screen.quad.vert.glsl",
			"assets/shaders/screen.quad.frag.glsl"
		);

		shader->Use();
		shader->SetInt("screen", 0); //Link render texture to the render program
	}

	void ScreenQuadRenderer::Draw(const std::shared_ptr<Texture>& tex) {

		shader->Use(); //Activate shader
		tex->Bind(); //Bind texture
		vao->Bind(); //Bind empty geometry
		
		glDrawArrays(GL_TRIANGLES, 0, 6); //Draw a screen squad
		tex->Unbind(); //Unind Texture
		vao->Unbind(); //Unind geometry
	}

}