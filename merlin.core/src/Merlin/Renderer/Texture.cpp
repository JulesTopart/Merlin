#include "glpch.h"
#include "Texture.h"
#include <stb_image.h>
#include<filesystem>

namespace Merlin::Renderer {

	Texture::Texture() : _type(Type::SPECULAR) {}
	Texture::~Texture() {}

	void Texture::Create() {
		glGenTextures(1, &_TextureID);
	}

	void Texture::Delete() {
		glDeleteTextures(1, &_TextureID);
	}

	void Texture::Bind() {
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, _TextureID);
	}

	void Texture::Unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::SetInterpolation(GLuint settingMin, GLuint settingMag) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settingMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settingMag);
	}
	void Texture::SetRepeat(GLuint setting) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, setting);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, setting);
	}

	void Texture::SetBorderColor(float colors[4]) {
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colors);
	}

	void Texture::SetBorderColor(float R, float G, float B, float A) {
		float colors[4] = { R,G,B,A };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colors);
	}

	void Texture::Load(const std::string img_file_path, Type t, GLuint format) {
		_type = t;
		unit = t == Type::DIFFUSE ? 0 : 1;

		int widthImg, heightImg, numColCh;
		// Flips the image so it appears right side up
		stbi_set_flip_vertically_on_load(true);

		unsigned char* bytes = stbi_load(img_file_path.c_str(), &widthImg, &heightImg, &numColCh, 0);

		Bind();

		SetBorderColor(0.0f, 0.0f, 0.0f, 1.0f);
		SetInterpolation(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		SetRepeat(GL_CLAMP_TO_BORDER);


		if (bytes != NULL) {
			Console::error("Texture") << "Cannot load image : " << std::filesystem::current_path().string() << img_file_path << Console::endl;
		}
		else {
			Console::info("Texture") << "Texture : (" << img_file_path << ") loaded sucessfully." << Console::endl;
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);
			glGenerateMipmap(GL_TEXTURE_2D);

		}
		Unbind();
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

	void Texture::textureUnit(Shader& shader, const std::string uniform, GLuint u) {
		//shader.Use();
		GLuint textureUnit = shader.GetUniformLocation(uniform.c_str());
		glUniform1i(textureUnit, u);
	}

}