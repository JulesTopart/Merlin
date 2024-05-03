#include "glpch.h"
#include "textureMultisampled.h"
#include <stb_image.h>
#include <filesystem>

namespace Merlin {
	TextureMultisampled2D::TextureMultisampled2D(GLuint samples, TextureType t) : TextureBase(GL_TEXTURE_2D_MULTISAMPLE, t), m_samples(samples){}

	void TextureMultisampled2D::reserve(GLuint width, GLuint height, GLuint channels, GLuint bits) {
		// Update the dimensions of the texture
		m_width = width;
		m_height = height;
		m_format = GL_RGB;
		m_internalFormat = GL_RGB8;

		// Determine format and internal format based on channels and bits
		switch (channels) {
		case 1:
			m_format = GL_RED;
			m_internalFormat = (bits == 32) ? GL_R32F : GL_R8;
			break;
		case 3:
			m_format = GL_RGB;
			m_internalFormat = (bits == 32) ? GL_RGB32F : GL_RGB8;
			break;
		case 4:
			m_format = GL_RGBA;
			m_internalFormat = (bits == 32) ? GL_RGBA32F : GL_RGBA8;
			break;
		}

		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_samples, m_internalFormat, m_width, m_height, GL_TRUE);

	}

	void TextureMultisampled2D::resize(GLsizei width, GLsizei height) {
		// Update the dimensions of the texture
		m_width = width;
		m_height = height;

		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_samples, m_internalFormat, m_width, m_height, GL_TRUE);
	}

	Shared<TextureMultisampled2D> TextureMultisampled2D::create(GLuint width, GLuint height, GLuint samples, TextureType t){
		Shared<TextureMultisampled2D> tex = createShared<TextureMultisampled2D>(samples, t);

		ChannelsProperty cb = TextureBase::getChannelsProperty(t);

		tex->bind();
		tex->reserve(width, height, cb.channels, cb.bits);
		tex->unbind();
		return tex;
	}

}