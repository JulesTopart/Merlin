#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Graphics/RendererBase.h"
#include "Merlin/Memory/Texture.h"
#include "Merlin/Memory/VertexArray.h"

using namespace Merlin::Memory;

namespace Merlin::Graphics {
	class ScreenQuadRenderer : public RendererBase{
	public:
		// Constructor
		ScreenQuadRenderer();

		// Render the screen quad
		void Render(const Shared<TextureBase>& tex);

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


