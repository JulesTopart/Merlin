#include "glpch.h"
#include "TextureMultisampled.h"
#include <stb_image.h>
#include <filesystem>

namespace Merlin::Memory {
	TextureMultisampled::TextureMultisampled(int samples) : _samples(samples){
		_Target = GL_TEXTURE_2D_MULTISAMPLE;
	}

	TextureMultisampled::~TextureMultisampled(){}

	void TextureMultisampled::Resize(GLsizei width, GLsizei height) {
		// Update the dimensions of the texture
		_width = width;
		_height = height;

		// Bind the texture
		Bind();
		// Resize the texture using glTexImage2D
		glTexImage2DMultisample(_Target, _samples, _format, _width, _height, GL_TRUE);
	}


	void TextureMultisampled::Allocate(int width, int height, GLenum format) {
		// Update the dimensions of the texture
		_width = width;
		_height = height;

		// Resize the texture using glTexImage2D

		glTexImage2DMultisample(_Target, _samples, _format, _width, _height, GL_TRUE);
	
	}

	void TextureMultisampled::LoadFromFile(const std::string img_file_path, TextureType t, GLenum format) {

		int widthImg, heightImg, numColCh;
		// Flips the image so it appears right side up
		stbi_set_flip_vertically_on_load(true);

		unsigned char* bytes = stbi_load(img_file_path.c_str(), &widthImg, &heightImg, &numColCh, 0);

		if (bytes == nullptr) {
			Console::error("Texture") << "Cannot load image : " << std::filesystem::current_path().string() << "/" << img_file_path << Console::endl;
		}
		else {
			Console::info("Texture") << "Texture : (" << img_file_path << ") loaded sucessfully." << Console::endl;

			LoadFromData(bytes, widthImg, heightImg, t, format);
			GenerateMipMap();
		}
		stbi_image_free(bytes);
	}

	void TextureMultisampled::LoadFromData(unsigned char* data, int width, int height, TextureType t, GLenum format) {
		_type = t;
		_width = width;
		_height = height;
		_format = format;

		glTexImage2DMultisample(_Target, _samples, format, _width, _height, GL_TRUE);
	}

}