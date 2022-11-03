#pragma once
#include <glad/glad.h>
#include "Shader.h"

namespace Merlin::Renderer {
	class Texture {
	public:

		Texture();
		~Texture();

		enum class Type {
			SPECULAR, DIFFUSE
		};


		void Create();
		void Delete();
		void Bind();
		void Unbind();

		void SetInterpolation(GLuint, GLuint);
		void SetRepeat(GLuint setting);

		void SetBorderColor(float colors[4]);
		void SetBorderColor(float R, float G, float B, float A);

		void Load(const std::string img_file_path, Type t = Type::DIFFUSE, GLuint format = GL_RGBA);
		void textureUnit(Shader&, const std::string uniform, GLuint unit);

		inline const Type type() const { return _type; }
		std::string typeToString() const;

	private:
		Type _type;
		GLuint unit = 0;
		GLuint _TextureID = 0;
	};

}