#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/indexBuffer.h"
#include "merlin/memory/vertexBuffer.h"
#include <vector>

namespace Merlin {

    class VertexArray : GLObject<> {
    public:
        VertexArray();
        
        void bind() const;
        void unbind() const;

        void bindBuffer(IndexBuffer& ib);

        template<class T = Vertex>
        void bindBuffer(VertexBuffer<T>& vb, const VertexBufferLayout& layout);

        template<class T = Vertex>
        void addBuffer(VertexBuffer<T>& vb, const VertexBufferLayout& layout);

    private:
        GLuint create();
        static void destroy(GLuint ID);
    };

    using VAO = VertexArray;


    template<class T>
    void VertexArray::bindBuffer(VertexBuffer<T>& vb, const VertexBufferLayout& layout) {
        glVertexArrayVertexBuffer(id(), vb.bindingPoint(), vb.id(), 0, layout.getStride());
    }


    template<class T>
    void VertexArray::addBuffer(VertexBuffer<T>& vb, const VertexBufferLayout& layout) {
	    const auto& elements = layout.getElements();
	    unsigned int offset = 0;

	    for (unsigned int i = 0; i < elements.size(); i++) {
		    const auto& element = elements[i];
            glEnableVertexArrayAttrib(id(), i);
            glVertexArrayAttribBinding(id(), i, vb.bindingPoint());
            glVertexArrayAttribFormat(id(), i, element.count, element.type, element.normalized, offset);
            offset += element.count * VertexBufferElement::getTypeSize(element.type);
	    }

        bindBuffer<T>(vb, layout);

    }
}