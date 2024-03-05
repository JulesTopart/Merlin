#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Graphics/RendererBase.h"
#include "Merlin/Memory/Texture.h"
#include "Merlin/Memory/VertexArray.h"

namespace Merlin {
	class ScreenQuadRenderer : public RendererBase {
	public:
		// Constructor
		ScreenQuadRenderer();

		// Render the screen quad
		void render();
		void render(const Shared<TextureBase>& tex);

		inline VAO& getVAO() { return m_vao; }
		inline Shader& getShader() { return m_shader; }
		inline void setShader(Shader& shader) { m_shader = shader;  m_shader.use(); m_shader.setInt("screen", 0); }

	private:
		//Empty vertex array object for the binding quad
		VAO m_vao;
		// Shader program for rendering the screen quad
		Shader m_shader;
	};

	typedef ScreenQuadRenderer SQRenderer;
	typedef Shared<ScreenQuadRenderer> ScreenQuadRenderer_Ptr;
}


