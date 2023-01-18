#pragma once
#include "Merlin/Core/Core.h"

#include "VertexBuffer.h"
#include <vector>

namespace Merlin::Renderer {

    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        void Bind();
        void Unbind();

        void AddBuffer(VertexBuffer& vb, const VertexBufferLayout& layout);
    private:
        GLuint m_RendererID;
    };

    typedef VertexArray VAO;
}