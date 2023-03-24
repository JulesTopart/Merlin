#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Graphics/Shader.h"

using namespace Merlin::Graphics;

namespace Merlin::Memory {

	enum class TextureType {
		SPECULAR, DIFFUSE, DEPTH, NORMAL, HEIGHT, ENVIRONMENT, LIGHT, SHADOW
	};

	class TextureBase {
	public:

		TextureBase();
		~TextureBase();

		void Bind();
		void Bind(GLuint unit);
		void Unbind();

		void SetUnit(GLuint unit);
		void SyncTextureUnit(const Shader& shader, const std::string uniform);

		virtual void Allocate(int width, int height, GLenum format = GL_RGBA) = 0;
		virtual void Resize(GLsizei width, GLsizei height) = 0;

		inline const GLenum GetTarget() const { return _Target; }
		inline const TextureType type() const { return _type; }
		std::string typeToString() const;

		inline const GLuint id() const { return _TextureID; }
	protected:

		GLenum _Target; // Either GL_TEXTURE_2D_MULTISAMPLE or GL_TEXTURE_2D or GL_TEXTURE_CUBEMAP
		GLuint _width = 0, _height = 0;
		GLenum _format;
		TextureType _type = TextureType::DIFFUSE;

	private:

		GLuint _unit = 0;
		GLuint _TextureID = 0;
	};


	class Texture : public TextureBase{
	public:

		Texture();
		~Texture();

		void SetInterpolationMode(GLuint minFilter = GL_LINEAR, GLuint magFilter = GL_LINEAR);
		void SetRepeatMode(GLuint _wrapS = GL_CLAMP_TO_BORDER, GLuint _wrapT = GL_CLAMP_TO_BORDER);

		void SetBorderColor4f(float colors[4]);
		void SetBorderColor4f(float R, float G, float B, float A);

		void GenerateMipMap();

		void Allocate(int width, int height, GLenum format = GL_RGBA) override;
		void Resize(GLsizei width, GLsizei height) override;
		void LoadFromFile(const std::string img_file_path, TextureType t = TextureType::DIFFUSE, GLenum format = GL_RGBA);
		void LoadFromData(unsigned char* data, int width, int height, TextureType t = TextureType::DIFFUSE, GLenum format = GL_RGBA);

	private:

		GLuint _minFilter, _magFilter, _wrapS, _wrapT;
	};

}