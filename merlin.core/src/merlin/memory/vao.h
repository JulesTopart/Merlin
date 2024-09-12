#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/ibo.h"
#include "merlin/memory/vbo.h"
#include "merlin/memory/ssbo.h"
#include <vector>

namespace Merlin {

    class VertexArray : public GLObject<> {
    public:
        VertexArray();

        inline void bind() const { glBindVertexArray(id()); }
        inline void unbind() const { glBindVertexArray(0); }

        void bindBuffer(IndexBuffer& ib);

        template<class T = Vertex>
        void bindBuffer(VertexBuffer<T>& vb, const VertexBufferLayout& layout);

        template<class T = Vertex>
        void addBuffer(VertexBuffer<T>& vb, const VertexBufferLayout& layout);

    private:
        GLuint create();
        static void destroy(GLuint ID);

        GLuint nextBindingPoint = 0;
    
    };


    using VAO = VertexArray;
    using VAO_Ptr = Shared<VertexArray>;

    template<class T>
    void VertexArray::bindBuffer(VertexBuffer<T>& vb, const VertexBufferLayout& layout) {
        glVertexArrayVertexBuffer(id(), vb.bindingPoint(), vb.id(), 0, layout.getStride());
    }

    template<class T>
    void VertexArray::addBuffer(VertexBuffer<T>& vb, const VertexBufferLayout& layout) {
        const auto& elements = layout.getElements();
        unsigned int offset = 0;
        vb.bind();
        vb.setBindingPoint(nextBindingPoint++);

        for (unsigned int i = 0; i < elements.size(); i++) {
            const auto& element = elements[i];
            glEnableVertexArrayAttrib(id(), i);
            glVertexArrayAttribFormat(id(), i, element.count, element.type, element.normalized, offset);
            glVertexArrayAttribBinding(id(), i, vb.bindingPoint());
            offset += element.count * VertexBufferElement::getTypeSize(element.type);
        }

        bindBuffer<T>(vb, layout);
        vb.unbind();
    }
}