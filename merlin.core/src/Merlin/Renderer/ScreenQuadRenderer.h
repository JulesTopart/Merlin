#pragma once
#include "Texture.h"
#include "VertexArray.h"

namespace Merlin::Renderer {
	class ScreenQuadRenderer {
	public:
		// Constructor
		ScreenQuadRenderer();

		// Render the screen quad
		void Draw(const std::shared_ptr<Texture>& tex);

	private:
		//Empty vertex array object for the binding quad
		std::unique_ptr<VAO> vao;
		// Shader program for rendering the screen quad
		std::unique_ptr<Shader> shader;
	};
}
