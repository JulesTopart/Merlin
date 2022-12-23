#pragma once
#include <glad/glad.h>
#include "Shader.h"

namespace Merlin::Renderer {
	class Texture {
	public:

		Texture(GLuint samples = 0);
		~Texture();

		enum class Type {
			SPECULAR, DIFFUSE, DEPTH, NORMAL, HEIGHT, ENVIRONMENT, LIGHT, SHADOW
		};

		void Bind();
		void Bind(GLuint unit);
		void Unbind();

		void SetInterpolationMode(GLuint minFilter = GL_LINEAR, GLuint magFilter = GL_LINEAR);
		void SetRepeatMode(GLuint _wrapS = GL_CLAMP_TO_BORDER, GLuint _wrapT = GL_CLAMP_TO_BORDER);

		void SetBorderColor4f(float colors[4]);
		void SetBorderColor4f(float R, float G, float B, float A);

		void LoadFromFile(const std::string img_file_path, Type t = Type::DIFFUSE, GLenum format = GL_RGBA);
		void LoadFromData(unsigned char* data, int width, int height, Type t = Type::DIFFUSE, GLenum format = GL_RGBA);

		void Resize(GLsizei width, GLsizei height);
		void GenerateMipMap();
		void SetUnit(GLuint unit);
		void SyncTextureUnit(Shader&, const std::string uniform);

		inline const GLenum GetTarget() const { return _Target; }
		inline const Type type() const { return _type; }
		std::string typeToString() const;

		inline const GLuint id() const { return _TextureID; }
	private:
		Type _type;
		GLuint _samples;
		GLuint _unit = 0;
		GLenum _format;
		GLuint _TextureID = 0;
		GLenum _Target; // Either GL_TEXTURE_2D_MULTISAMPLE or GL_TEXTURE_2D
		GLuint _width = 0, _height = 0;

		GLuint _minFilter, _magFilter, _wrapS, _wrapT;
	};

}