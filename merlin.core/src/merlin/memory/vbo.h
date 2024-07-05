#pragma once
#include "merlin/core/core.h"
#include "merlin/memory/bufferObject.h"
#include "merlin/memory/vertex.h"

namespace Merlin {

    //constexpr GLenum DEFAULT_VERTEX_BUFFER_INTERNAL_FORMAT = GL_FLOAT;

    template<class T = Vertex>
    class VertexBuffer : public ImmutableBufferObject<T> {
    public:
        VertexBuffer(GLsizeiptr size);
        VertexBuffer(const std::vector<T>& vertices);
        VertexBuffer(const std::string& name, GLsizeiptr size);
        VertexBuffer(const std::string& name, const std::vector<T>& vertices);
    };

    template<class T>
    inline VertexBuffer<T>::VertexBuffer(GLsizeiptr size) : ImmutableBufferObject<T>(BufferTarget::Array_Buffer) {
        this->allocate(size, BufferStorageFlags::DynamicStorage);
        Console::trace("VertexBuffer") << "VertexBuffer " << this->id() << " allocated. " << Console::endl;
    }

    template<class T>
    inline VertexBuffer<T>::VertexBuffer(const std::vector<T>& vertices) : ImmutableBufferObject<T>(BufferTarget::Array_Buffer) {
        this->allocate(vertices, BufferStorageFlags::DynamicStorage);
        Console::trace("VertexBuffer") << "VertexBuffer " << this->id() << " allocated. " << Console::endl;
    }

    template<class T>
    inline VertexBuffer<T>::VertexBuffer(const std::string& name, GLsizeiptr size) : ImmutableBufferObject<T>(BufferTarget::Array_Buffer) {
        this->allocate(size, BufferStorageFlags::DynamicStorage);
        this->rename(name);
        Console::trace("VertexBuffer") << "VertexBuffer " << name << " " << this->id() << " allocated. " << Console::endl;
    }

    template<class T>
    inline VertexBuffer<T>::VertexBuffer(const std::string& name, const std::vector<T>& vertices) : ImmutableBufferObject<T>(BufferTarget::Array_Buffer) {
        this->allocate(vertices.size(), vertices, BufferStorageFlags::DynamicStorage);
        this->rename(name);
        Console::trace("VertexBuffer") << "VertexBuffer " << name << " " << this->id() << " allocated. " << Console::endl;
    }

    template<class T = Vertex>
    using VBO = VertexBuffer<T>; // Shorter alias

    template<class T = Vertex>
    using VBO_Ptr = std::shared_ptr<VertexBuffer<T>>; // Shorter alias
    

}