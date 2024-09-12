#pragma once
#include "merlin/core/core.h"
#include "merlin/textures/texture.h"

namespace Merlin {

	class TextureMultisampled2D : public TextureBase{
	public:
		TextureMultisampled2D(GLuint samples = 4, TextureType t = TextureType::ALBEDO);

		//Memory
		void reserve(GLuint width, GLuint height, GLuint depth = 0, GLuint channels = 3, GLuint bits = 8) override;
		void resize(GLuint width, GLuint height, GLuint depth = 0) override;

		static Shared<TextureMultisampled2D> create(GLuint width, GLuint height, GLuint samples, TextureType = TextureType::ALBEDO);
	private:
		GLuint m_samples;
	};


}
