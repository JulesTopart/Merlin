#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Memory/Texture.h"
#include "Merlin/Memory/VertexArray.h"

using namespace Merlin::Memory;

namespace Merlin::Graphics {
	class ScreenQuadRenderer {
	public:
		// Constructor
		ScreenQuadRenderer();

		// Render the screen quad
		void Draw(const Shared<Texture>& tex);

		inline std::shared_ptr<VAO> GetVAO(){ return vao; }
		inline std::shared_ptr<Shader> GetShader() { return shader; }

	private:
		//Empty vertex array object for the binding quad
		std::shared_ptr<VAO> vao;
		// Shader program for rendering the screen quad
		std::shared_ptr<Shader> shader;
	};

	typedef ScreenQuadRenderer SQRenderer;
}


