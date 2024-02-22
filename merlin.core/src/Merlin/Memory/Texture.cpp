#include "glpch.h"
#include "Texture.h"
#include <stb_image.h>
#include <filesystem>
#include "Merlin/Core/Log.h"

namespace Merlin {

	TextureBase::TextureBase(GLenum target, TextureType t) : _type(t), _Target(target), _format(GL_RGB), _internalFormat(GL_RGB8) {
		//Set the target based on the number of samples
		glGenTextures(1, &_TextureID);
	}

	TextureBase::~TextureBase() {
		glDeleteTextures(1, &_TextureID);
	}

	void TextureBase::Bind() {
		// Activate the appropriate texture unit (offsetting from Texture0 using the _unit)
		glActiveTexture(GL_TEXTURE0 + _unit);
		// Bind the texture to the appropriate target
		glBindTexture(_Target, _TextureID);
	}

	void TextureBase::Bind(GLuint unit) {
		// Activate the appropriate texture unit (offsetting from Texture0 using the unit parameter)
		glActiveTexture(GL_TEXTURE0 + unit);
		// Bind the texture to the appropriate target
		glBindTexture(_Target, _TextureID);

	}

	void TextureBase::BindImage() {
		// Activate the appropriate texture unit (offsetting from Texture0 using the _unit)
		glActiveTexture(GL_TEXTURE0 + _unit);
		// Bind the texture to the appropriate target
		glBindImageTexture(_unit, _TextureID,0, GL_FALSE, 0, GL_READ_WRITE, _internalFormat);
	}

	void TextureBase::BindImage(GLuint unit) {
		// Activate the appropriate texture unit (offsetting from Texture0 using the _unit)
		glActiveTexture(GL_TEXTURE0 + unit);
		// Bind the texture to the appropriate target
		glBindImageTexture(_unit, _TextureID, 0, GL_FALSE, 0, GL_READ_WRITE, _internalFormat);
	}

	void TextureBase::Unbind() {
		// Unbind the texture to the appropriate target
		glBindTexture(_Target, 0);

	}

	void TextureBase::SetUnit(GLuint unit) {
		_unit = unit;
	}


	void TextureBase::SyncTextureUnit(const ShaderBase& shader, const std::string uniform) {
		shader.SetInt(uniform, _unit);
	}


	std::string TextureBase::typeToString() const {
		if (type() == TextureType::COLOR) {
			return "color";
		}
		else if (type() == TextureType::ROUGHNESS) {
			return "roughness";
		}
		else if (type() == TextureType::NORMAL) {
			return "normal";
		}
		else if (type() == TextureType::HEIGHT) {
			return "height";
		}
		else if (type() == TextureType::DEPTH) {
			return "depth";
		}
		else if (type() == TextureType::EMISSION) {
			return "emission";
		}
		else if (type() == TextureType::SHADOW) {
			return "shadow";
		}
		else if (type() == TextureType::AMBIENT_OCCLUSION) {
			return "ambient_occlusion";
		}
		else {
			Console::error("Texture") << "Invalid texture type" << Console::endl;
		}
		return "ERROR";
	}


	Texture::Texture() : TextureBase(GL_TEXTURE_2D, TextureType::COLOR) {
		//Set the target based on the number of samples
	}


	Texture::Texture(TextureType t) : TextureBase(GL_TEXTURE_2D, t) {
		//Set the target based on the number of samples
	}

	Texture::~Texture() {}

	void Texture::GenerateMipMap() {
		glGenerateMipmap(GetTarget());
	}

	void Texture::SetInterpolationMode(GLuint settingMin, GLuint settingMag) {
		if (GetTarget() == GL_TEXTURE_2D_MULTISAMPLE) {
			Console::error("Texture") << "You cannot set interpolation filter on a multisampled texture" << Console::endl;
			return;
		}
		glTexParameteri(GetTarget(), GL_TEXTURE_MIN_FILTER, settingMin);
		glTexParameteri(GetTarget(), GL_TEXTURE_MAG_FILTER, settingMag);
	}

	void Texture::SetRepeatMode(GLuint _wrapS, GLuint _wrapT) {
		if (GetTarget() == GL_TEXTURE_2D_MULTISAMPLE) {
			Console::error("Texture") << "You cannot set warp behavior on a multisampled texture" << Console::endl;
			return;
		}
		glTexParameteri(GetTarget(), GL_TEXTURE_WRAP_S, _wrapS);
		glTexParameteri(GetTarget(), GL_TEXTURE_WRAP_T, _wrapT);
	}

	void Texture::SetBorderColor4f(float colors[4]) {
		
		if (GetTarget() == GL_TEXTURE_2D_MULTISAMPLE) {
			Console::error("Texture") << "You cannot set a border color on a multisampled texture" << Console::endl;
			return;
		}
		glTexParameterfv(GetTarget(), GL_TEXTURE_BORDER_COLOR, colors);
	}

	void Texture::SetBorderColor4f(float R, float G, float B, float A) {
		if (GetTarget() == GL_TEXTURE_2D_MULTISAMPLE) {
			Console::error("Texture") << "You cannot set a border color on a multisampled texture" << Console::endl;
			return;
		}
		
		float colors[4] = { R,G,B,A };
		glTexParameterfv(GetTarget(), GL_TEXTURE_BORDER_COLOR, colors);
	}

	void Texture::Resize(GLsizei width, GLsizei height) {
		// Update the dimensions of the texture
		_width = width;
		_height = height;

		// Bind the texture
		Bind();
		// Resize the texture using glTexImage2D
		glTexImage2D(GetTarget(), 0, _format, width, height, 0, _format, GL_UNSIGNED_BYTE, nullptr);
	}

	void Texture::Allocate(int width, int height, GLenum format, GLenum internalformat) {
		// Update the dimensions of the texture
		_width = width;
		_height = height;
		_format = format;
		if (internalformat == GL_INVALID_ENUM)_internalFormat = GL_RGBA32F;
		else _internalFormat = internalformat;

		// Resize the texture using glTexImage2D
		LoadFromData(nullptr, width, height, format);
		SetInterpolationMode(GL_LINEAR, GL_LINEAR);
		SetRepeatMode(GL_REPEAT, GL_REPEAT);
		//glTexImage2D(GetTarget(), 0, _internalFormat, _width, _height, 0, _format, GL_UNSIGNED_BYTE, data);
	}

	void Texture::LoadFromFile(const std::string img_file_path) {
		int widthImg, heightImg, nrChannels;

		// Flips the image so it appears right side up
		stbi_set_flip_vertically_on_load(true);

		unsigned char* bytes = stbi_load(img_file_path.c_str(), &widthImg, &heightImg, &nrChannels, 0);

		GLenum format = GL_RGB;
		if (nrChannels == 4) format = GL_RGBA;

		if (bytes == nullptr) {
			Console::error("Texture") << "Cannot load image : " << std::filesystem::current_path().string() << "/" << img_file_path << Console::endl;
		}else {
			Console::info("Texture") << "Texture : (" << img_file_path << ") loaded sucessfully." << Console::endl;

			LoadFromData(bytes, widthImg, heightImg, format);
			GenerateMipMap();
		}
		stbi_image_free(bytes);
	}



	void Texture::LoadFromData(unsigned char* data, int width, int height, GLenum format) {
		_width = width;
		_height = height;
		_format = format;
		_internalFormat = _format == GL_RGBA ? GL_RGBA32F : format;

		glTexImage2D(GetTarget(), 0, _internalFormat, _width, _height, 0, _format, GL_UNSIGNED_BYTE, data);
	}





}