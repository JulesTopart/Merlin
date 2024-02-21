#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Graphics/RendererBase.h"
#include "Merlin/Memory/Texture.h"
#include "Merlin/Memory/VertexArray.h"

using namespace Merlin::Memory;

namespace Merlin::Graphics {
	class ScreenQuadRenderer : public RendererBase {
	public:
		// Constructor
		ScreenQuadRenderer();

		// Render the screen quad
		void Render();
		void Render(const Shared<TextureBase>& tex);

		inline VAO& GetVAO() { return m_vao; }
		inline Shader& GetShader() { return m_shader; }
		inline void SetShader(Shader& shader) { m_shader = shader;  m_shader.Use(); m_shader.SetInt("screen", 0); }

	private:
		//Empty vertex array object for the binding quad
		VAO m_vao;
		// Shader program for rendering the screen quad
		Shader m_shader;
	};

	typedef ScreenQuadRenderer SQRenderer;
	typedef Shared<ScreenQuadRenderer> ScreenQuadRenderer_Ptr;
}


