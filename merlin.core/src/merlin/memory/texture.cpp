#include "glpch.h"
#include "texture.h"
#include <stb_image.h>
#include <filesystem>

#include "merlin/utils/util.h"
#include "merlin/utils/textureLoader.h"
#include "merlin/core/log.h"

namespace Merlin {

	TextureBase::TextureBase(GLenum target, TextureType t, TextureClass c) : m_type(t), m_class(c), m_Target(target), m_format(GL_RGB), m_internalFormat(GL_RGB8) {
		//Set the target based on the number of samples
		glGenTextures(1, &m_TextureID);
	}

	TextureBase::~TextureBase() {
		glDeleteTextures(1, &m_TextureID);
	}

	void TextureBase::bind() {
		// Activate the appropriate texture unit (offsetting from Texture0 using the m_unit)
		glActiveTexture(GL_TEXTURE0 + m_unit);
		// bind the texture to the appropriate target
		glBindTexture(m_Target, m_TextureID);


	}

	void TextureBase::bind(GLuint unit) {
		// Activate the appropriate texture unit (offsetting from Texture0 using the unit parameter)
		glActiveTexture(GL_TEXTURE0 + unit);
		// bind the texture to the appropriate target
		glBindTexture(m_Target, m_TextureID);

	}

	void TextureBase::bindImage() {
		// Activate the appropriate texture unit (offsetting from Texture0 using the m_unit)
		glActiveTexture(GL_TEXTURE0 + m_unit);
		// bind the texture to the appropriate target
		glBindImageTexture(m_unit, m_TextureID,0, GL_FALSE, 0, GL_READ_WRITE, m_internalFormat);
	}

	void TextureBase::bindImage(GLuint unit) {
		// Activate the appropriate texture unit (offsetting from Texture0 using the m_unit)
		glActiveTexture(GL_TEXTURE0 + unit);
		// bind the texture to the appropriate target
		glBindImageTexture(m_unit, m_TextureID, 0, GL_FALSE, 0, GL_READ_WRITE, m_internalFormat);
	}

	void TextureBase::unbind() {
		// unbind the texture to the appropriate target
		glActiveTexture(GL_TEXTURE0 + m_unit);
		glBindTexture(m_Target, 0);
		

	}

	void TextureBase::setUnit(GLuint unit) {
		
		//Console::warn("TextureBase") << "Texture[" << id() << "] bound to " << m_unit << "->" << unit << Console::endl;
		m_unit = unit;
	}

	void TextureBase::autoSetUnit() {
		
		auto it = textureUnitMap.find(id());
		if (it != textureUnitMap.end()) {
			setUnit(it->second);
		}
		else {
			setUnit(currentTextureUnit++);
			textureUnitMap[id()] = m_unit; // Store the binding point in the map
		}

	}

	void TextureBase::resetTextureUnits() {
		TextureBase::currentTextureUnit = 0;
		TextureBase::textureUnitMap.clear();
	}

	void TextureBase::syncTextureUnit(const ShaderBase& shader, const std::string uniform) {
		shader.setInt(uniform, m_unit);
	}


	std::string TextureBase::typeToString() const {
		if (type() == TextureType::ALBEDO) {
			return "color";
		}
		else if (type() == TextureType::ROUGHNESS) {
			return "roughness";
		}
		else if (type() == TextureType::NORMAL) {
			return "normal";
		}
		else if (type() == TextureType::REFLECTION) {
			return "reflection";
		}
		else if (type() == TextureType::MASK) {
			return "mask";
		}
		else if (type() == TextureType::METALNESS) {
			return "metalness";
		}
		else if (type() == TextureType::DISPLACMENT) {
			return "displacement";
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
		else if (type() == TextureType::ENVIRONMENT) {
			return "environment";
		}
		else {
			Console::error("Texture") << "Invalid texture type" << Console::endl;
		}
		return "ERROR";
	}

	ChannelsProperty TextureBase::getChannelsProperty(TextureType t)
	{
		GLuint bits = 8;
		GLuint channels = 3;

		switch (t) {
		case TextureType::ALBEDO: //COL sRGB
			bits = 8;
			channels = 3; //RGB
			break;
		case TextureType::NORMAL: //NRM RGB
			bits = 8;
			channels = 3;//XYZ
			break;
		case TextureType::DISPLACMENT: //DISP R 16 displacement
			bits = 16;
			channels = 1;//R
			break;
		case TextureType::REFLECTION://sRGB (Specularity)
			bits = 8;
			channels = 3;
			break;
		case TextureType::ROUGHNESS://Linear 8 _ROUGHNESS (Glossiness)
			bits = 8;
			channels = 1;
			break;
		case TextureType::METALNESS://Linear 8 _ROUGHNESS (Glossiness)
			bits = 8;
			channels = 1;
			break;
		case TextureType::AMBIENT_OCCLUSION:
			bits = 8;
			channels = 1;
			break;
		case TextureType::EMISSION:
			bits = 8;
			channels = 3;
			break;
		case TextureType::DEPTH:
			bits = 16;
			channels = 1;
			break;
		case TextureType::SHADOW:
			bits = 16;
			channels = 1;
			break;
		case TextureType::ENVIRONMENT:
			bits = 32;
			channels = 3;
			break;
		case TextureType::DATA:
			bits = 32;
			channels = 1;
			break;
		case TextureType::UNKNOWN:
			break;
		default:
			break;
		}

		return { channels, bits };
	}

	Texture2D::Texture2D(TextureType t) : TextureBase(GL_TEXTURE_2D, t, TextureClass::TEXTURE2D) {}

	void Texture2D::generateMipmap() const{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void Texture2D::setInterpolationMode(GLuint settingMin, GLuint settingMag) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settingMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settingMag);
	}

	void Texture2D::setRepeatMode(GLuint _wrapS, GLuint _wrapT) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrapT);
	}

	void Texture2D::setBorderColor4f(float colors[4]) {
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colors);
	}

	void Texture2D::setBorderColor4f(float R, float G, float B, float A) {	
		float colors[4] = { R,G,B,A };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colors);
	}


	void Texture2D::reserve(GLuint width, GLuint height, GLuint channels, GLuint bits){
		ImageData data;
		data.bits = bits;
		data.width = width;
		data.height = height;
		data.channels = channels;
		data.bytes = nullptr;
		data.dataType = GL_UNSIGNED_BYTE;
		loadFromData(data);
	}

	void Texture2D::reserve(GLuint width, GLuint height, GLenum format, GLenum internalFormat, GLenum type) {
		m_width = width;
		m_height = height;
		m_format = format;
		m_internalFormat = internalFormat;

		glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, width, height, 0, m_format, type, nullptr);
	}

	void Texture2D::resize(GLsizei width, GLsizei height) {
		// Update the dimensions of the texture
		m_width = width;
		m_height = height;

		glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, width, height, 0, m_format, m_dataType, nullptr);
	}

	void Texture2D::loadFromData(const ImageData& data)	{

		m_width = data.width;
		m_height = data.height;
		m_dataType = data.dataType;
		m_format = GL_RGB;
		m_internalFormat = GL_RGB8;

		// Determine format and internal format based on channels and bits
		switch (data.channels) {
		case 1:
			m_format = GL_RED;
			m_internalFormat = (data.bits == 32) ? GL_R32F : GL_R8;
			break;
		case 3:
			m_format = GL_RGB;
			m_internalFormat = (data.bits == 32) ? GL_RGB32F : GL_RGB8;
			break;
		case 4:
			m_format = GL_RGBA;
			m_internalFormat = (data.bits == 32) ? GL_RGBA32F : GL_RGBA8;
			break;
		}

		// upload the texture data
		glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_format, m_dataType, data.bytes);
		generateMipmap();
	}

	void Texture2D::loadFromFile(const std::string& path){
		loadFromData(TextureLoader::loadImageData(path));
	}

	Shared<Texture2D> Texture2D::create(GLuint width, GLuint height, TextureType t){
		Shared<Texture2D> tex = createShared<Texture2D>(t);
		tex->bind();

		if (t == TextureType::SHADOW || t == TextureType::DEPTH) {
			tex->reserve(width, height, GL_DEPTH_STENCIL, GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8);
			tex->setInterpolationMode(GL_NEAREST, GL_NEAREST);
			tex->setRepeatMode(GL_REPEAT, GL_REPEAT);
		}else {		
			ChannelsProperty cp = TextureBase::getChannelsProperty(t);
			tex->reserve(width, height, cp.channels, cp.bits);
			tex->setInterpolationMode();
			tex->setRepeatMode();
		}

		tex->unbind();
		return tex;
	}

	Shared<Texture2D> Texture2D::create(const ImageData& data, TextureType t){
		Shared<Texture2D> tex = createShared<Texture2D>(t);
		tex->bind();
		tex->loadFromData(data);
		tex->setInterpolationMode();
		tex->setRepeatMode();
		tex->generateMipmap();
		tex->unbind();
		return tex;
	}

	Shared<Texture2D> Texture2D::create(const std::string& path, TextureType t){
		ImageData data = TextureLoader::loadImageData(path);
		
		Shared<Texture2D> tex = createShared<Texture2D>(t);
		tex->bind();
		tex->loadFromData(data);
		// Clean up if necessary
		if (data.bytes) {
			stbi_image_free(data.bytes);
		}
		tex->unbind();
		return tex;
	}
}