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

		void Bind();
		void Bind(GLuint unit);
		void BindImage();
		void BindImage(GLuint unit);
		void Unbind();


		void SetUnit(GLuint unit);
		void SyncTextureUnit(const ShaderBase& shader, const std::string uniform);

		virtual void Allocate(int width, int height, GLenum format = GL_RGBA, GLenum internalformat = GL_INVALID_ENUM) = 0;
		virtual void Resize(GLsizei width, GLsizei height) = 0;

		inline const GLenum GetFormat() const { return _format; }
		inline const GLenum GetTarget() const { return _Target; }
		inline const TextureType type() const { return _type; }
		std::string typeToString() const;

		inline bool IsDefault() { return _width == 1 && _height == 1; }

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

		void SetInterpolationMode(GLuint minFilter = GL_LINEAR, GLuint magFilter = GL_LINEAR);
		void SetRepeatMode(GLuint _wrapS = GL_CLAMP_TO_BORDER, GLuint _wrapT = GL_CLAMP_TO_BORDER);

		void SetBorderColor4f(float colors[4]);
		void SetBorderColor4f(float R, float G, float B, float A);

		void GenerateMipMap();

		void Allocate(int width, int height, GLenum format = GL_RGBA, GLenum internalformat = GL_INVALID_ENUM) override;
		void Resize(GLsizei width, GLsizei height) override;
		void LoadFromFile(const std::string img_file_path);
		void LoadFromData(unsigned char* data, int width, int height, GLenum format = GL_RGBA);

	private:

		GLuint _minFilter, _magFilter, _wrapS, _wrapT;
	};

	typedef Shared<Texture> Texture_Ptr;
}