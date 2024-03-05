#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/Shader.h"

namespace Merlin {

	enum class TextureType {
		COLOR, NORMAL, ROUGHNESS, AMBIENT_OCCLUSION, EMISSION, DEPTH, HEIGHT, SHADOW
	};

	class TextureBase {
	public:

		TextureBase(GLenum target, TextureType t = TextureType::COLOR);
		~TextureBase();

		void bind();
		void bind(GLuint unit);
		void bindImage();
		void bindImage(GLuint unit);
		void unbind();


		void setUnit(GLuint unit);
		void syncTextureUnit(const ShaderBase& shader, const std::string uniform);

		virtual void reserve(int width, int height, GLenum format = GL_RGBA, GLenum internalformat = GL_INVALID_ENUM) = 0;
		virtual void resize(GLsizei width, GLsizei height) = 0;

		inline const GLenum getFormat() const { return _format; }
		inline const GLenum getTarget() const { return _Target; }
		inline const TextureType type() const { return _type; }
		std::string typeToString() const;

		inline bool isDefault() { return _width == 1 && _height == 1; }

		inline const GLuint id() const { return _TextureID; }
	protected:

		GLuint _width = 0, _height = 0;
		GLenum _format, _internalFormat;
		TextureType _type = TextureType::COLOR;

	private:

		GLenum _Target; // Either GL_TEXTURE_2D_MULTISAMPLE or GL_TEXTURE_2D or GL_TEXTURE_CUBEMAP
		GLuint _unit = 0;
		GLuint _TextureID = 0;
	};


	class Texture : public TextureBase{
	public:

		Texture();
		Texture(TextureType t);
		~Texture();

		void setInterpolationMode(GLuint minFilter = GL_LINEAR, GLuint magFilter = GL_LINEAR);
		void setRepeatMode(GLuint _wrapS = GL_CLAMP_TO_BORDER, GLuint _wrapT = GL_CLAMP_TO_BORDER);

		void setBorderColor4f(float colors[4]);
		void setBorderColor4f(float R, float G, float B, float A);

		void generateMipMap();

		void reserve(int width, int height, GLenum format = GL_RGBA, GLenum internalformat = GL_INVALID_ENUM) override;
		void resize(GLsizei width, GLsizei height) override;
		void loadFromFile(const std::string img_file_path);
		void loadFromData(unsigned char* data, int width, int height, GLenum format = GL_RGBA);

	private:

		GLuint _minFilter, _magFilter, _wrapS, _wrapT;
	};

	typedef Shared<Texture> Texture_Ptr;
}