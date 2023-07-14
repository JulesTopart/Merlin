#include "glpch.h"
#include "ScreenQuadRenderer.h"

using namespace Merlin::Memory;

namespace Merlin::Graphics {
	ScreenQuadRenderer::ScreenQuadRenderer() {
		
		vao = CreateShared<VAO>();

		shader = CreateShared<Shader>("screen");
		shader->Compile(
			"assets/shaders/screen.vert.glsl",
			"assets/shaders/screen.frag.glsl"
		);

		shader->Use();
		shader->SetInt("screen", 0); //Link render texture to the render program
	}

	void ScreenQuadRenderer::Render(const Shared<TextureBase>& tex) {

		shader->Use(); //Activate shader
		tex->Bind(); //Bind texture
		tex->SyncTextureUnit(*shader, "screen");
		vao->Bind(); //Bind empty geometry
		

		glDrawArrays(GL_TRIANGLES, 0, 6); //Draw a screen squad
		tex->Unbind(); //Unind Texture
		vao->Unbind(); //Unind geometry
	}

}