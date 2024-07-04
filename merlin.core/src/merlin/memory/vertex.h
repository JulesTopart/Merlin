#pragma once
#include "merlin/core/core.h"

#include <vector>

namespace Merlin {

    struct VertexBufferElement {
        unsigned int type;
        unsigned int count;
        unsigned char normalized;

        static unsigned int getTypeSize(unsigned int type) {
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
        void push(unsigned int count) {
            assert(false);
        }

        template<>
        void push<float>(unsigned int count) {
            m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
            m_Stride += VertexBufferElement::getTypeSize(GL_FLOAT) * count;
        }

        template<>
        void push<unsigned int>(unsigned int count) {
            m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
            m_Stride += VertexBufferElement::getTypeSize(GL_UNSIGNED_INT) * count;
        }

        template<>
        void push<unsigned char>(unsigned int count) {
            m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
            m_Stride += VertexBufferElement::getTypeSize(GL_BYTE) * count;
        }

        inline const std::vector<VertexBufferElement>& getElements() const { return m_Elements; }

        inline unsigned int getStride() const { return m_Stride; }

    private:
        std::vector<VertexBufferElement> m_Elements;
        GLuint m_Stride;
    };

	struct Vertex {
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 normal = glm::vec3(0.0f);
		glm::vec3 color = glm::vec3(1.0f);
		glm::vec2 texCoord = glm::vec2(0.0f, 0.0f);
		glm::vec3 tangent = glm::vec3(0.0f);
		glm::vec3 bitangent = glm::vec3(0.0f);

		Vertex() {};
		Vertex(glm::vec3 position_,
			glm::vec3 normal_ = glm::vec3(0.0),
			glm::vec3 color_ = glm::vec3(1.0),
			glm::vec2 texCoord_ = glm::vec2(0.0),
			glm::vec3 tangent_ = glm::vec3(0.0),
			glm::vec3 bitangent_ = glm::vec3(0.0)) :
			position(position_), normal(normal_), color(color_), texCoord(texCoord_), tangent(tangent_), bitangent(bitangent_) {};

		static VertexBufferLayout getLayout();
	};
    typedef std::vector<Vertex> Vertices;


	inline VertexBufferLayout Vertex::getLayout() {
		VertexBufferLayout layout;
		layout.push<float>(3); //Vertex pos
		layout.push<float>(3); //Vertex normal
		layout.push<float>(3); //Vertex color
		layout.push<float>(2); //Texture coordinates
		layout.push<float>(3); //tangent
		layout.push<float>(3); //bitangent
		return layout;
	}


    


}