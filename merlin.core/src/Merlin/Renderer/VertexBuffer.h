#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace Merlin::Renderer {

    class VertexBufferLayout;

	struct Vertex {
		glm::vec3
			position = glm::vec3(0.0f, 0.0f, 0.0f),
			normal = glm::vec3(0.0f, 0.0f, 0.0f),
			color = glm::vec3(0.0f, 0.0f, 0.0f);

		glm::vec2
			textureUV = glm::vec2(0.0f, 0.0f);

		static VertexBufferLayout GetLayout();
	};

	class VertexBuffer {
	public:
		VertexBuffer(std::vector<Vertex>& vertices);
		~VertexBuffer();

		void Bind();
		void Unbind();

	private:
		unsigned int m_RendererID;
	};

	typedef VertexBuffer VBO;


    struct VertexBufferElement {
        unsigned int type;
        unsigned int count;
        unsigned char normalized;

        static unsigned int GetTypeSize(unsigned int type) {
            switch (type) {
            case GL_FLOAT:          return 4;
            case GL_UNSIGNED_INT:   return 4;
            case GL_UNSIGNED_BYTE:  return 1;
            }
            return 0;
        }
    };

    class VertexBufferLayout {
    public:
        VertexBufferLayout() : m_Stride(0) {}

        template<typename T>
        void Push(unsigned int count) {
            static_assert(false);
        }

        template<>
        void Push<float>(unsigned int count) {
            m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
            m_Stride += VertexBufferElement::GetTypeSize(GL_FLOAT) * count;
        }

        template<>
        void Push<unsigned int>(unsigned int count) {
            m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
            m_Stride += VertexBufferElement::GetTypeSize(GL_UNSIGNED_INT) * count;
        }

        template<>
        void Push<unsigned char>(unsigned int count) {
            m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
            m_Stride += VertexBufferElement::GetTypeSize(GL_BYTE) * count;
        }

        inline const std::vector<VertexBufferElement>
            GetElements() const { return m_Elements; }

        inline unsigned int
            GetStride() const { return m_Stride; }

    private:
        std::vector<VertexBufferElement> m_Elements;
        GLuint m_Stride;
    };

}