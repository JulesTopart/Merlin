#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Renderer/Texture.h"

namespace Merlin::Renderer {

	class TextureMultisampled : public Texture{
	public:
			TextureMultisampled(int samples = 4);
			~TextureMultisampled();

			void Resize(GLsizei width, GLsizei height) override;
			void Allocate(int width, int height, GLenum format = GL_RGBA) override;

			void LoadFromFile(const std::string img_file_path, TextureType t = TextureType::DIFFUSE, GLenum format = GL_RGBA);
			void LoadFromData(unsigned char* data, int width, int height, TextureType t = TextureType::DIFFUSE, GLenum format = GL_RGBA);


		private:
			GLuint _samples;
	};


}
