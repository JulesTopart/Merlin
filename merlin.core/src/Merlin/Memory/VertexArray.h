#pragma once
#include "Merlin/Core/Core.h"
#include "Merlin/Memory/VertexBuffer.h"
#include <vector>

namespace Merlin {

    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();
        
        void Bind() const;
        void Unbind() const;

        template<class T = Vertex>
        void AddBuffer(VertexBuffer<T>& vb, const VertexBufferLayout& layout);

    private:
        
        GLuint ArrayID = -1;
    };

    using VAO = VertexArray;


    template<class T>
    void VertexArray::AddBuffer(VertexBuffer<T>& vb, const VertexBufferLayout& layout) {
	    vb.Bind();
	    const auto& elements = layout.GetElements();
	    unsigned int offset = 0;
	    for (unsigned int i = 0; i < elements.size(); i++) {
		    const auto& element = elements[i];
		    glEnableVertexAttribArray(i);
		    glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset);
		    offset += element.count * VertexBufferElement::GetTypeSize(element.type);
	    }
	    vb.Unbind();
    }
}