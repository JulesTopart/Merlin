#include "glpch.h"
#include "textureMultisampled.h"
#include <stb_image.h>
#include <filesystem>

namespace Merlin {
	TextureMultisampled::TextureMultisampled(TextureType t, int samples) : TextureBase(GL_TEXTURE_2D_MULTISAMPLE, t), _samples(samples){}

	TextureMultisampled::~TextureMultisampled(){}

	void TextureMultisampled::resize(GLsizei width, GLsizei height) {
		// Update the dimensions of the texture
		_width = width;
		_height = height;

		// bind the texture
		bind();
		// resize the texture using glTexImage2D
		glTexImage2DMultisample(getTarget(), _samples, _format, _width, _height, GL_TRUE);
	}


	void TextureMultisampled::reserve(int width, int height, GLenum format, GLenum internalformat) {
		// Update the dimensions of the texture
		_width = width;
		_height = height;
		_format = format;
		if (internalformat == GL_INVALID_ENUM) _internalFormat = _format;

		// resize the texture using glTexImage2D

		glTexImage2DMultisample(getTarget(), _samples, _format, _width, _height, GL_TRUE);
	
	}

	void TextureMultisampled::loadFromFile(const std::string img_file_path, GLenum format) {

		int widthImg, heightImg, numColCh;
		// Flips the image so it appears right side up
		stbi_set_flip_vertically_on_load(true);

		unsigned char* bytes = stbi_load(img_file_path.c_str(), &widthImg, &heightImg, &numColCh, 0);

		if (bytes == nullptr) {
			Console::error("Texture") << "Cannot load image : " << std::filesystem::current_path().string() << "/" << img_file_path << Console::endl;
		}
		else {
			Console::info("Texture") << "Texture : (" << img_file_path << ") loaded sucessfully." << Console::endl;

			loadFromData(bytes, widthImg, heightImg, format);
		}
		stbi_image_free(bytes);
	}

	void TextureMultisampled::loadFromData(unsigned char* data, int width, int height, GLenum format) {
		_width = width;
		_height = height;
		_format = format;
		//_internalFormat = _format == GL_RGBA ? GL_RGBA32F : format;

		glTexImage2DMultisample(getTarget(), _samples, _internalFormat, _width, _height, GL_TRUE);
	}

}