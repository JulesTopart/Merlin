#pragma once
#include "../Renderer/Mesh.h"

namespace Merlin::Utils::MeshLoader{
	/*
	std::shared_ptr<Renderer::Mesh> LoadObject(std::string filepath, std::string name = "3D Object");
		 */

	std::shared_ptr<Renderer::Mesh> LoadFromBinaries(std::string filepath, std::string name = "3D Object");

    class GeometryLayout;

    struct GeometryData {
        std::vector<GLfloat> verticies;
        std::vector<GLuint> indices;
        static GeometryLayout GetLayout(); //Keep that static
    };


    struct GeometryElement {
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

    class GeometryLayout {
    public:
        GeometryLayout() : m_Stride(0) {}

        template<typename T>
        void Push(unsigned int count) {
            static_assert(false);
        }

        template<>
        void Push<float>(unsigned int count) {
            m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
            m_Stride += GeometryElement::GetTypeSize(GL_FLOAT) * count;
        }

        template<>
        void Push<unsigned int>(unsigned int count) {
            m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
            m_Stride += GeometryElement::GetTypeSize(GL_UNSIGNED_INT) * count;
        }

        template<>
        void Push<unsigned char>(unsigned int count) {
            m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
            m_Stride += GeometryElement::GetTypeSize(GL_BYTE) * count;
        }

        inline const std::vector<GeometryElement>
            GetElements() const { return m_Elements; }

        inline unsigned int
            GetStride() const { return m_Stride; }

    private:
        std::vector<GeometryElement> m_Elements;
        GLuint m_Stride;
    };

}