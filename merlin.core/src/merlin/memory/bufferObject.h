#pragma once
#include "merlin/core/core.h"
#include "merlin/core/console.h"


#include <vector>
#include <glm/glm.hpp>
#include <glad/gl.h>

namespace Merlin {
    // Enums for buffer usage, storage flags, and binding points
    enum class BufferUsage : GLenum {
        StreamDraw = GL_STREAM_DRAW,
        StreamRead = GL_STREAM_READ,
        StreamCopy = GL_STREAM_COPY,
        StaticDraw = GL_STATIC_DRAW,
        StaticRead = GL_STATIC_READ,
        StaticCopy = GL_STATIC_COPY,
        DynamicDraw = GL_DYNAMIC_DRAW,
        DynamicRead = GL_DYNAMIC_READ,
        DynamicCopy = GL_DYNAMIC_COPY
    };

    enum class BufferStorageFlags : GLbitfield {
        DynamicStorage = GL_DYNAMIC_STORAGE_BIT,
        MapRead = GL_MAP_READ_BIT,
        MapWrite = GL_MAP_WRITE_BIT,
        MapPersistent = GL_MAP_PERSISTENT_BIT,
        MapCoherent = GL_MAP_COHERENT_BIT,
        ClientStorage = GL_CLIENT_STORAGE_BIT
    };

    enum class BufferTarget : GLenum {
        Array_Buffer = GL_ARRAY_BUFFER,
        Atomic_Counter_Buffer = GL_ATOMIC_COUNTER_BUFFER,
        Element_Array_Buffer = GL_ELEMENT_ARRAY_BUFFER,
        Uniform_Buffer = GL_UNIFORM_BUFFER,
        Shader_Storage_Buffer = GL_SHADER_STORAGE_BUFFER,
        Copy_Read_Buffer = GL_COPY_READ_BUFFER,
        Copy_Write_Buffer = GL_COPY_WRITE_BUFFER,
        Dispatch_Indirect_Buffer = GL_DISPATCH_INDIRECT_BUFFER,
        Draw_Indirect_Buffer = GL_DRAW_INDIRECT_BUFFER,
        Pixel_PackBuffer = GL_PIXEL_PACK_BUFFER,
        Pixel_Unpack_Buffer = GL_PIXEL_UNPACK_BUFFER,
        Query_Buffer = GL_QUERY_BUFFER,
        Texture_Buffer = GL_TEXTURE_BUFFER,
        Transform_Feedback_Buffer = GL_TRANSFORM_FEEDBACK_BUFFER
    };

    // Base class for buffer objects
    class AbstractBufferObject : public GLObject<>{
    public:
        AbstractBufferObject(BufferTarget target);

        void* map(GLenum access) const;
        void unmap() const;

        inline void bind() const { glBindBuffer(static_cast<GLenum>(m_target), id()); }
        inline void bindAs(GLenum target) const { glBindBuffer(static_cast<GLenum>(target), id()); }
        inline void unbind() const { glBindBuffer(static_cast<GLenum>(m_target), 0); }

        void allocateBuffer(GLsizeiptr size, const void* data, BufferUsage usage);
        void allocateImmutableBuffer(GLsizeiptr size, const void* data, BufferStorageFlags flags);

        void writeBuffer(GLsizeiptr size, const void* data);
        void readBuffer(GLsizeiptr size, void* data) const;

        void resizeBuffer(GLsizeiptr size);
        void clearBuffer() const;

        inline BufferUsage usage() const { return m_usage; }
        inline BufferTarget target() const { return m_target; }
        inline BufferStorageFlags flags() const { return m_flags; }

        inline GLenum bindingPoint() const { return m_bindingPoint; }
        void setBindingPoint(GLuint point);
        void releaseBindingPoint();

        inline GLsizeiptr size() const { return m_size; }
        inline GLuint type() const { return m_type; }
        inline GLsizeiptr elements() const { return m_elements; }

        inline void setType(GLuint typeSize) { m_type = typeSize; }
        inline void setElements(GLsizeiptr elements) { m_elements = elements; }

    protected:
        BufferUsage m_usage;
        BufferTarget m_target;
        BufferStorageFlags m_flags;
        GLuint m_bindingPoint = -1;

        bool m_isMutable = true;
        bool m_isAllocated = false;

    private:
        GLsizeiptr m_size = 0;      //buffer size in bytes
        GLsizeiptr m_elements = 0;  //nb of element size in buffer
        GLuint m_type = 1;  //size of elements in bytes

        void checkMutable() const;
        GLuint create();
        static void destroy(GLuint id);
    };

    typedef Shared<AbstractBufferObject> AbstractBufferObject_Ptr;
    
    template <typename T>
    class BufferObject : public AbstractBufferObject {
    public:
        BufferObject(BufferTarget target);

        void allocate(GLsizeiptr size, BufferUsage usage);
        void allocate(GLsizeiptr size, T* data, BufferUsage usage);
        void allocate(std::vector<T> data, BufferUsage usage);

        void write(const std::vector<T>& data);
        void write(const T* data, GLsizeiptr size);
        void clear();

        void read(std::vector<T>& data) const;
        std::vector<T> read() const;
    };


    template <typename T>
    class ImmutableBufferObject : public AbstractBufferObject {
    public:
        ImmutableBufferObject(BufferTarget target);

        void allocate(GLsizeiptr size, BufferStorageFlags flags);
        void allocate(GLsizeiptr size, T* data, BufferStorageFlags flags);
        void allocate(std::vector<T> data, BufferStorageFlags flags);
        void clear();

        void read(std::vector<T>& data) const;
        std::vector<T> read() const;
    };
}

#include "bufferObject.inl"