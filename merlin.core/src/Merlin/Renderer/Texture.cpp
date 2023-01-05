#include "glpch.h"
#include "Texture.h"
#include <stb_image.h>
#include<filesystem>

namespace Merlin::Renderer {

	Texture::Texture(GLuint samples) : _samples(samples), _format(GL_RGB) {
		//Set the target based on the number of samples
		glGenTextures(1, &_TextureID);
		
		if (samples > 0) {
			_Target = GL_TEXTURE_2D_MULTISAMPLE;

		}else {
			_Target = GL_TEXTURE_2D;
		}
	}

	Texture::~Texture() {
		glDeleteTextures(1, &_TextureID);
	}

	void Texture::Bind() {
		// Activate the appropriate texture unit (offsetting from Texture0 using the _unit)
		glActiveTexture(GL_TEXTURE0 + _unit);
		// Bind the texture to the appropriate target
		glBindTexture(_Target, _TextureID);
	}

	void Texture::Bind(GLuint unit) {
		// Activate the appropriate texture unit (offsetting from Texture0 using the unit parameter)
		glActiveTexture(GL_TEXTURE0 + unit);
		// Bind the texture to the appropriate target
		glBindTexture(_Target, _TextureID);
		
	}

	void Texture::Unbind() {
		// Unbind the texture to the appropriate target
		glBindTexture(_Target, 0);

	}

	void Texture::SetInterpolationMode(GLuint settingMin, GLuint settingMag) {
		if (_samples > 0) Console::error("Texture") << "You cannot set interpolation filter on a multisampled texture" << Console::endl;
		glTexParameteri(_Target, GL_TEXTURE_MIN_FILTER, settingMin);
		glTexParameteri(_Target, GL_TEXTURE_MAG_FILTER, settingMag);
	}

	void Texture::SetRepeatMode(GLuint _wrapS, GLuint _wrapT) {
		if (_samples > 0) Console::error("Texture") << "You cannot set warp behavior on a multisampled texture" << Console::endl;
		glTexParameteri(_Target, GL_TEXTURE_WRAP_S, _wrapS);
		glTexParameteri(_Target, GL_TEXTURE_WRAP_T, _wrapT);
	}

	void Texture::SetBorderColor4f(float colors[4]) {
		if (_samples > 0) Console::error("Texture") << "You cannot set a border color on a multisampled texture" << Console::endl;
		glTexParameterfv(_Target, GL_TEXTURE_BORDER_COLOR, colors);
	}

	void Texture::SetBorderColor4f(float R, float G, float B, float A) {
		if (_samples > 0) Console::error("Texture") << "You cannot set a border color on a multisampled texture" << Console::endl;
		float colors[4] = { R,G,B,A };
		glTexParameterfv(_Target, GL_TEXTURE_BORDER_COLOR, colors);
	}

	void Texture::Resize(GLsizei width, GLsizei height) {
		// Update the dimensions of the texture
		_width = width;
		_height = height;

		// Bind the texture
		Bind();
		// Resize the texture using glTexImage2D
		if (_samples > 0)
			glTexImage2DMultisample(_Target, _samples, _format, _width, _height, GL_TRUE);
		else
			glTexImage2D(_Target, 0, _format, width, height, 0, _format, GL_UNSIGNED_BYTE, nullptr);
	}

	void Texture::Allocate(int width, int height, GLenum format) {
		// Update the dimensions of the texture
		_width = width;
		_height = height;

		// Resize the texture using glTexImage2D
		if (_samples > 0)
			glTexImage2DMultisample(_Target, _samples, _format, _width, _height, GL_TRUE);
		else
			glTexImage2D(_Target, 0, _format, width, height, 0, _format, GL_UNSIGNED_BYTE, nullptr);
	}

	void Texture::LoadFromFile(const std::string img_file_path, Type t, GLenum format) {

		int widthImg, heightImg, numColCh;
		// Flips the image so it appears right side up
		stbi_set_flip_vertically_on_load(true);

		unsigned char* bytes = stbi_load(img_file_path.c_str(), &widthImg, &heightImg, &numColCh, 0);
	
		if (bytes == nullptr) {
			Console::error("Texture") << "Cannot load image : " << std::filesystem::current_path().string() << "/" << img_file_path << Console::endl;
		}else {
			Console::info("Texture") << "Texture : (" << img_file_path << ") loaded sucessfully." << Console::endl;

			LoadFromData(bytes, widthImg, heightImg, t, format);
			if (_samples == 0) GenerateMipMap();
		}
		//stbi_image_free(bytes);
	}

	void Texture::LoadFromData(unsigned char* data, int width, int height, Type t, GLenum format) {
		_type = t;
		_width = width;
		_height = height;
		_format = format;

		if(_samples > 0)
			glTexImage2DMultisample(_Target, _samples, format, _width, _height, GL_TRUE);
		else
			glTexImage2D(_Target, 0, _format, _width, _height, 0, _format, GL_UNSIGNED_BYTE, data);
	}

	void Texture::GenerateMipMap() {
		glGenerateMipmap(_Target);
	}

	std::string Texture::typeToString() const {
		if (type() == Texture::Type::DIFFUSE) {
			return "diffuse";
		}
		else if (type() == Texture::Type::SPECULAR) {
			return "specular";
		}
		else {
			Console::error("Texture") << "Invalid texture type" << Console::endl;
		}
		return "ERROR";
	}

	void Texture::SetUnit(GLuint unit) {
		_unit = unit;
	}

	void Texture::SyncTextureUnit(std::shared_ptr<Shader> shader, const std::string uniform) {
		GLuint textureUnit = shader->GetUniformLocation(uniform.c_str());
		glUniform1i(textureUnit, _unit);
	}

}