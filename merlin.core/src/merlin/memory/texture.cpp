#include "glpch.h"
#include "texture.h"
#include <stb_image.h>
#include <filesystem>
#include "merlin/core/log.h"

namespace Merlin {

	TextureBase::TextureBase(GLenum target, TextureType t) : _type(t), _Target(target), _format(GL_RGB), _internalFormat(GL_RGB8) {
		//Set the target based on the number of samples
		glGenTextures(1, &_TextureID);
	}

	TextureBase::~TextureBase() {
		glDeleteTextures(1, &_TextureID);
	}

	void TextureBase::bind() {
		// Activate the appropriate texture unit (offsetting from Texture0 using the _unit)
		glActiveTexture(GL_TEXTURE0 + _unit);
		// bind the texture to the appropriate target
		glBindTexture(_Target, _TextureID);
	}

	void TextureBase::bind(GLuint unit) {
		// Activate the appropriate texture unit (offsetting from Texture0 using the unit parameter)
		glActiveTexture(GL_TEXTURE0 + unit);
		// bind the texture to the appropriate target
		glBindTexture(_Target, _TextureID);

	}

	void TextureBase::bindImage() {
		// Activate the appropriate texture unit (offsetting from Texture0 using the _unit)
		glActiveTexture(GL_TEXTURE0 + _unit);
		// bind the texture to the appropriate target
		glBindImageTexture(_unit, _TextureID,0, GL_FALSE, 0, GL_READ_WRITE, _internalFormat);
	}

	void TextureBase::bindImage(GLuint unit) {
		// Activate the appropriate texture unit (offsetting from Texture0 using the _unit)
		glActiveTexture(GL_TEXTURE0 + unit);
		// bind the texture to the appropriate target
		glBindImageTexture(_unit, _TextureID, 0, GL_FALSE, 0, GL_READ_WRITE, _internalFormat);
	}

	void TextureBase::unbind() {
		// unbind the texture to the appropriate target
		glBindTexture(_Target, 0);

	}

	void TextureBase::setUnit(GLuint unit) {
		_unit = unit;
	}


	void TextureBase::syncTextureUnit(const ShaderBase& shader, const std::string uniform) {
		shader.setInt(uniform, _unit);
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


	Texture::Texture() : TextureBase(GL_TEXTURE_2D, TextureType::COLOR) {}
	Texture::Texture(TextureType t) : TextureBase(GL_TEXTURE_2D, t) {}
	Texture::Texture(GLenum target, TextureType t) : TextureBase(target, t) {}

	Shared<Texture> Texture::create() {
		return createShared<Texture>();
	}

	Shared<Texture> Texture::create(TextureType t) {
		return createShared<Texture>(t);
	}

	Shared<Texture> Texture::create(GLenum target, TextureType t) {
		return createShared<Texture>(target, t);
	}

	Shared<Texture> Texture::create(const std::string img_file_path, TextureType type) {
		Texture_Ptr result = createShared<Texture>(type);
		result->loadFromFile(img_file_path);
		return result;
	}

	Shared<Texture> Texture::create(GLuint width, GLuint height, GLenum format, GLenum internalformat, TextureType type) {
		Texture_Ptr result = createShared<Texture>(type);
		result->reserve(width, height, format, internalformat);
		return result;
	}

	void Texture::generateMipMap() const{
		glGenerateMipmap(getTarget());
	}

	void Texture::setInterpolationMode(GLuint settingMin, GLuint settingMag) {
		if (getTarget() == GL_TEXTURE_2D_MULTISAMPLE) {
			Console::error("Texture") << "You cannot set interpolation filter on a multisampled texture" << Console::endl;
			return;
		}
		glTexParameteri(getTarget(), GL_TEXTURE_MIN_FILTER, settingMin);
		glTexParameteri(getTarget(), GL_TEXTURE_MAG_FILTER, settingMag);
	}

	void Texture::setRepeatMode(GLuint _wrapS, GLuint _wrapT) {
		if (getTarget() == GL_TEXTURE_2D_MULTISAMPLE) {
			Console::error("Texture") << "You cannot set warp behavior on a multisampled texture" << Console::endl;
			return;
		}
		glTexParameteri(getTarget(), GL_TEXTURE_WRAP_S, _wrapS);
		glTexParameteri(getTarget(), GL_TEXTURE_WRAP_T, _wrapT);
	}

	void Texture::setBorderColor4f(float colors[4]) {
		
		if (getTarget() == GL_TEXTURE_2D_MULTISAMPLE) {
			Console::error("Texture") << "You cannot set a border color on a multisampled texture" << Console::endl;
			return;
		}
		glTexParameterfv(getTarget(), GL_TEXTURE_BORDER_COLOR, colors);
	}

	void Texture::setBorderColor4f(float R, float G, float B, float A) {
		if (getTarget() == GL_TEXTURE_2D_MULTISAMPLE) {
			Console::error("Texture") << "You cannot set a border color on a multisampled texture" << Console::endl;
			return;
		}
		
		float colors[4] = { R,G,B,A };
		glTexParameterfv(getTarget(), GL_TEXTURE_BORDER_COLOR, colors);
	}

	void Texture::resize(GLsizei width, GLsizei height) {
		// Update the dimensions of the texture
		_width = width;
		_height = height;

		// bind the texture
		bind();
		// resize the texture using glTexImage2D
		glTexImage2D(getTarget(), 0, _format, width, height, 0, _format, GL_UNSIGNED_BYTE, nullptr);
	}

	void Texture::reserve(int width, int height, GLenum format, GLenum internalformat) {
		// Update the dimensions of the texture
		_width = width;
		_height = height;
		_format = format;
		if (internalformat == GL_INVALID_ENUM)_internalFormat = GL_RGBA32F;
		else _internalFormat = internalformat;

		// resize the texture using glTexImage2D
		loadFromData(nullptr, width, height, format);
		setInterpolationMode(GL_LINEAR, GL_LINEAR);
		setRepeatMode(GL_REPEAT, GL_REPEAT);
		//glTexImage2D(getTarget(), 0, _internalFormat, _width, _height, 0, _format, GL_UNSIGNED_BYTE, data);
	}

	void Texture::loadFromFile(const std::string img_file_path) {
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

			loadFromData(bytes, widthImg, heightImg, format);
			generateMipMap();
		}
		stbi_image_free(bytes);
	}



	void Texture::loadFromData(unsigned char* data, int width, int height, GLenum format) {
		_width = width;
		_height = height;
		_format = format;
		if(_internalFormat == GL_RGBA) _internalFormat = GL_RGBA32F;

		glTexImage2D(getTarget(), 0, _internalFormat, _width, _height, 0, _format, GL_UNSIGNED_BYTE, data);
	}





}