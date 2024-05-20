#pragma once
#include "merlin/core/core.h"

#include "merlin/graphics/rendererBase.h"
#include "merlin/memory/texture.h"
#include "merlin/memory/vertexArray.h"

namespace Merlin {
	class ScreenQuadRenderer : public RendererBase {
	public:
		// Constructor
		ScreenQuadRenderer();

		// Render the screen quad
		void render();
		void render(const Shared<TextureBase>& tex);

		inline VAO& getVAO() { return m_vao; }
		inline Shader& getShader() { return *m_shader; }
		inline void setShader(Shared<Shader> shader) { m_shader = shader;  m_shader->use(); m_shader->setInt("screen", 0); }

	private:
		//Empty vertex array object for the binding quad
		VAO m_vao;
		// Shader program for rendering the screen quad
		Shared<Shader> m_shader;
	};

	typedef ScreenQuadRenderer SQRenderer;
	typedef Shared<ScreenQuadRenderer> ScreenQuadRenderer_Ptr;
}


