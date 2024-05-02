#pragma once
#include "merlin/core/core.h"
#include "merlin/shaders/shader.h"

namespace Merlin {

	enum class TextureType {
		COLOR, NORMAL, DISPLACMENT, REFLECTION, ROUGHNESS, METALNESS, AMBIENT_OCCLUSION, MASK, EMISSION, DEPTH, SHADOW, ENVIRONMENT, DATA, UNKNOWN
	};

	struct ImageData {
		GLuint width = 0;
		GLuint height = 0;
		GLuint channels = 0;
		GLuint bits = 8;  // Default to 8 bits per channel
		GLenum dataType = GL_UNSIGNED_BYTE; // Default to unsigned byte
		void* bytes = nullptr;
	};

	class TextureBase {
	public:

		TextureBase(GLenum target, TextureType t);
		~TextureBase();

		void bind();
		void bind(GLuint unit);
		void bindImage();
		void bindImage(GLuint unit);
		void unbind();

		void setUnit(GLuint unit);
		void syncTextureUnit(const ShaderBase& shader, const std::string uniform);

		virtual void reserve(GLuint width, GLuint height, GLuint channels = 3, GLuint bits = 8) = 0;
		virtual void resize(GLsizei width, GLsizei height) = 0;

		inline const GLenum getFormat() const { return m_format; }
		inline const GLenum getTarget() const { return m_Target; }
		inline const TextureType type() const { return m_type; }
		std::string typeToString() const;

		inline bool isDefault() { return m_width == 1 && m_height == 1; }

		inline const GLuint id() const { return m_TextureID; }
	protected:

		GLuint m_width = 0, m_height = 0;
		GLenum m_format, m_internalFormat, m_dataType;
		TextureType m_type = TextureType::COLOR;

	private:

		GLenum m_Target; // Either GL_TEXTURE_2D_MULTISAMPLE or GL_TEXTURE_2D or GL_TEXTURE_CUBEMAP
		GLuint m_unit = 0;
		GLuint m_TextureID = 0;
	};


	class Texture2D : public TextureBase{
	public:

		Texture2D(TextureType t);
		
		//Settings
		void setInterpolationMode(GLuint minFilter = GL_LINEAR, GLuint magFilter = GL_LINEAR);
		void setRepeatMode(GLuint _wrapS = GL_CLAMP_TO_BORDER, GLuint _wrapT = GL_CLAMP_TO_BORDER);
		void setBorderColor4f(float colors[4]);
		void setBorderColor4f(float R, float G, float B, float A);
		void generateMipmap() const;

		//Memory
		void reserve(GLuint width, GLuint height, GLuint channels = 3, GLuint bits = 8) override;
		void resize(GLsizei width, GLsizei height) override;
		void loadFromData(const ImageData& data);

		static Shared<Texture2D> create(GLuint width, GLuint height, TextureType = TextureType::COLOR);
		static Shared<Texture2D> create(const ImageData& data, TextureType = TextureType::COLOR);

	private:

		GLuint m_minFilter, m_magFilter, m_wrapS, m_wrapT;
	};

	typedef Shared<Texture2D> Texture_Ptr;
}