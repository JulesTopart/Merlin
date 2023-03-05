#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/VertexBuffer.h"
#include <vector>

namespace Merlin::Memory {

    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        void Bind() const;
        void Unbind() const;

        void AddBuffer(VertexBuffer& vb, const VertexBufferLayout& layout);
    private:
        GLuint ArrayID;
    };

    typedef VertexArray VAO;
}