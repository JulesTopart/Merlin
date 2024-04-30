#pragma once
#include "merlin/core/core.h"

#include "merlin/memory/texture.h"

namespace Merlin {

	class TextureMultisampled : public TextureBase{
	public:
			TextureMultisampled(TextureType t = TextureType::COLOR, int samples = 4);
			~TextureMultisampled();

			void resize(GLsizei width, GLsizei height) override;
			void reserve(int width, int height,  GLenum format = GL_RGBA, GLenum internalformat = GL_INVALID_ENUM) override;

			void loadFromFile(const std::string img_file_path, GLenum format = GL_RGBA);
			void loadFromData(unsigned char* data, int width, int height, GLenum format = GL_RGBA);


		private:
			GLuint _samples;
	};


}
