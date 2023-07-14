#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Memory/Texture.h"

namespace Merlin::Memory {

	class TextureMultisampled : public TextureBase{
	public:
			TextureMultisampled(TextureType t = TextureType::COLOR, int samples = 4);
			~TextureMultisampled();

			void Resize(GLsizei width, GLsizei height) override;
			void Allocate(int width, int height,  GLenum format = GL_RGBA, GLenum internalformat = GL_INVALID_ENUM) override;

			void LoadFromFile(const std::string img_file_path, GLenum format = GL_RGBA);
			void LoadFromData(unsigned char* data, int width, int height, GLenum format = GL_RGBA);


		private:
			GLuint _samples;
	};


}
