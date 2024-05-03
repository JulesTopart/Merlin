#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/texture.h"

namespace Merlin {

	class TextureMultisampled2D : public TextureBase{
	public:
		TextureMultisampled2D(GLuint samples = 4, TextureType t = TextureType::COLOR);

		//Memory
		void reserve(GLuint width, GLuint height, GLuint channels = 3, GLuint bits = 8) override;
		void resize(GLsizei width, GLsizei height) override;

		static Shared<TextureMultisampled2D> create(GLuint width, GLuint height, GLuint samples, TextureType = TextureType::COLOR);
	private:
		GLuint m_samples;
	};


}
