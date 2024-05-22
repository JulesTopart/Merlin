#pragma once
#include "merlin/core/core.h"
#include "merlin/shaders/shader.h"

namespace Merlin {

	enum class TextureType {
		DIFFUSE, SPECULAR, ALBEDO, NORMAL, DISPLACMENT, REFLECTION, ROUGHNESS, METALNESS, AMBIENT_OCCLUSION, MASK, EMISSION, DEPTH, SHADOW, ENVIRONMENT, DATA, UNKNOWN
	};


	enum class TextureClass {
		BLANK, TEXTURE2D, TEXTURE2D_MULTISAMPLED, CUBE_MAP, TEXTURE3D
	};

	struct ImageData {
		int width = 0;
		int height = 0;
		int channels = 0;
		int bits = 8;  // Default to 8 bits per channel
		GLenum dataType = GL_UNSIGNED_BYTE; // Default to unsigned byte
		void* bytes = nullptr;
	};

	struct ChannelsProperty {
		GLuint channels = 3;
		GLuint bits = 8;
	};

	class TextureBase {
	public:

		TextureBase(GLenum target, TextureType t, TextureClass c);
		~TextureBase();

		void bind();
		void bind(GLuint unit);
		void bindImage();
		void bindImage(GLuint unit);
		void unbind();

		void autoSetUnit();
		void setUnit(GLuint unit);
		void syncTextureUnit(const ShaderBase& shader, const std::string uniform);

		//virtual void reserve(GLuint width, GLuint height, GLenum format, GLenum internalFormat, GLenum type) = 0;
		virtual void reserve(GLuint width, GLuint height, GLuint channels = 3, GLuint bits = 8) = 0;
		virtual void resize(GLsizei width, GLsizei height) = 0;

		inline const GLenum getFormat() const { return m_format; }
		inline const GLenum getTarget() const { return m_Target; }
		inline const TextureType type() const { return m_type; }
		inline const TextureClass textureClass() const { return m_class; }
		std::string typeToString() const;

		inline bool isDefault() { return m_width == 1 && m_height == 1; }
		static ChannelsProperty getChannelsProperty(TextureType);
		inline const GLuint id() const { return m_TextureID; }

		static void resetTextureUnits();
		

	protected:
		GLuint m_width = 0, m_height = 0;
		GLenum m_format, m_internalFormat, m_dataType;
		TextureType m_type = TextureType::ALBEDO;
		TextureClass m_class = TextureClass::BLANK;

	private:
		inline static GLuint currentTextureUnit = 0;
		inline static std::unordered_map<GLuint, GLuint> textureUnitMap; // Map to track binding points


		GLenum m_Target; // Either GL_TEXTURE_2D_MULTISAMPLE or GL_TEXTURE_2D or GL_TEXTURE_CUBEMAP
		GLuint m_unit = 0;
		GLuint m_TextureID = 0;
	};


	class Texture2D : public TextureBase{
	public:

		Texture2D(TextureType t = TextureType::ALBEDO);
		
		//Settings
		void setInterpolationMode(GLuint minFilter = GL_LINEAR, GLuint magFilter = GL_LINEAR);
		void setRepeatMode(GLuint _wrapS = GL_CLAMP_TO_EDGE, GLuint _wrapT = GL_CLAMP_TO_EDGE);
		void setBorderColor4f(float colors[4]);
		void setBorderColor4f(float R, float G, float B, float A);
		void generateMipmap() const;

		//Memory
		void reserve(GLuint width, GLuint height, GLenum format, GLenum internalFormat, GLenum type);
		void reserve(GLuint width, GLuint height, GLuint channels = 3, GLuint bits = 8) override;
		void resize(GLsizei width, GLsizei height) override;
		void loadFromData(const ImageData& data);
		void loadFromFile(const std::string& path);

		static Shared<Texture2D> create(GLuint width, GLuint height, TextureType = TextureType::ALBEDO);
		static Shared<Texture2D> create(const ImageData& data, TextureType = TextureType::ALBEDO);
		static Shared<Texture2D> create(const std::string& path, TextureType = TextureType::ALBEDO);

	
	private:	
		
		GLuint m_minFilter, m_magFilter, m_wrapS, m_wrapT;
	};

	typedef Shared<Texture2D> Texture2D_Ptr;
}