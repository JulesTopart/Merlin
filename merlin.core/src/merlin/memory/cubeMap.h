#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/texture.h"
#include <vector>


namespace Merlin{
	class CubeMap : public TextureBase {
	public:
		CubeMap(TextureType = TextureType::ALBEDO);

		//Settings
		void setInterpolationMode(GLuint minFilter = GL_LINEAR, GLuint magFilter = GL_LINEAR);
		void setRepeatMode(GLuint _wrapS = GL_CLAMP_TO_BORDER, GLuint _wrapT = GL_CLAMP_TO_BORDER, GLuint _wrapR = GL_CLAMP_TO_BORDER);
		void setBorderColor4f(float colors[4]);
		void setBorderColor4f(float R, float G, float B, float A);
		void generateMipmap() const;

		//Memory
		void reserve(GLuint width, GLuint height, GLenum format, GLenum internalFormat, GLenum type);
		void reserve(GLuint width, GLuint height, GLuint channels = 3, GLuint bits = 8) override;
		void resize(GLsizei width, GLsizei height) override;

		static Shared<CubeMap> create(GLuint width, GLuint height, TextureType = TextureType::ALBEDO);
		static Shared<CubeMap> create(const std::vector<std::string>& paths, TextureType = TextureType::ALBEDO);

	private:
		void loadFromFiles(const std::vector<std::string>& faces);


	};
}

