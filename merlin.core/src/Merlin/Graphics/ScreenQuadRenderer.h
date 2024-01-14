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

		inline VAO& GetVAO(){ return m_vao; }
		inline Shader& GetShader() { return m_shader; }

	private:
		//Empty vertex array object for the binding quad
		VAO m_vao;
		// Shader program for rendering the screen quad
		Shader m_shader;
	};

	typedef ScreenQuadRenderer SQRenderer;
}


