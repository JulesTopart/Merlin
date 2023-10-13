#include "glpch.h"
#include "TextureMultisampled.h"
#include <stb_image.h>
#include <filesystem>

namespace Merlin::Memory {
	TextureMultisampled::TextureMultisampled(TextureType t, int samples) : TextureBase(GL_TEXTURE_2D_MULTISAMPLE, t), _samples(samples){}

	TextureMultisampled::~TextureMultisampled(){}

	void TextureMultisampled::Resize(GLsizei width, GLsizei height) {
		// Update the dimensions of the texture
		_width = width;
		_height = height;

		// Bind the texture
		Bind();
		// Resize the texture using glTexImage2D
		glTexImage2DMultisample(GetTarget(), _samples, _format, _width, _height, GL_TRUE);
	}


	void TextureMultisampled::Allocate(int width, int height, GLenum format, GLenum internalformat) {
		// Update the dimensions of the texture
		_width = width;
		_height = height;
		_format = format;
		if (internalformat == GL_INVALID_ENUM) _internalFormat = _format;

		// Resize the texture using glTexImage2D

		glTexImage2DMultisample(GetTarget(), _samples, _format, _width, _height, GL_TRUE);
	
	}

	void TextureMultisampled::LoadFromFile(const std::string img_file_path, GLenum format) {

		int widthImg, heightImg, numColCh;
		// Flips the image so it appears right side up
		stbi_set_flip_vertically_on_load(true);

		unsigned char* bytes = stbi_load(img_file_path.c_str(), &widthImg, &heightImg, &numColCh, 0);

		if (bytes == nullptr) {
			Console::error("Texture") << "Cannot load image : " << std::filesystem::current_path().string() << "/" << img_file_path << Console::endl;
		}
		else {
			Console::info("Texture") << "Texture : (" << img_file_path << ") loaded sucessfully." << Console::endl;

			LoadFromData(bytes, widthImg, heightImg, format);
		}
		stbi_image_free(bytes);
	}

	void TextureMultisampled::LoadFromData(unsigned char* data, int width, int height, GLenum format) {
		_width = width;
		_height = height;
		_format = format;
		//_internalFormat = _format == GL_RGBA ? GL_RGBA32F : format;

		glTexImage2DMultisample(GetTarget(), _samples, _internalFormat, _width, _height, GL_TRUE);
	}

}