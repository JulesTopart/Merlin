#include "bufferObject.h"

namespace Merlin {
    // Base class for buffer objects
    inline AbstractBufferObject::AbstractBufferObject(BufferTarget target) : GLObject(create(), destroy), m_target(target) {}

    inline GLuint AbstractBufferObject::create() {
        GLuint ID;
        glCreateBuffers(1, &ID);
        return ID;
    }

    inline void AbstractBufferObject::destroy(GLuint ID) {
        glDeleteBuffers(1, &ID);
    }

    inline void* AbstractBufferObject::map(GLenum access) const {
        return glMapNamedBuffer(id(), access);
    }

    inline void AbstractBufferObject::unmap() const {
        glUnmapNamedBuffer(id());
    }

    inline void AbstractBufferObject::allocateBuffer(GLsizeiptr size, const void* data, BufferUsage usage) {
        m_isMutable = true;
        m_size = size;
        m_usage = usage;
        glNamedBufferData(id(), size, nullptr, static_cast<GLenum>(usage));
        clearBuffer();
        writeBuffer(size, data);
    }

    inline void AbstractBufferObject::allocateImmutableBuffer(GLsizeiptr size, const void* data, BufferStorageFlags flags) {
        m_isMutable = false;
        m_size = size;
        m_flags = flags;
        glNamedBufferStorage(id(), size, data, static_cast<GLbitfield>(flags));
    }

    inline void AbstractBufferObject::resizeBuffer(GLsizeiptr size) {
        checkMutable();
        m_size = size;
        glNamedBufferData(id(), size, nullptr, static_cast<GLenum>(m_usage));
    }

    inline void AbstractBufferObject::writeBuffer(GLsizeiptr size, const void* data) {
        checkMutable();
        glNamedBufferSubData(id(), 0, size, data);
    }

    inline void AbstractBufferObject::readBuffer(GLsizeiptr size, void* data) const {
        glGetNamedBufferSubData(id(), 0, size, data);
    }


    inline void AbstractBufferObject::clearBuffer() const {
        checkMutable();
        GLubyte val = 0;
        glClearNamedBufferData(id(), GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &val);
    }

    inline void AbstractBufferObject::checkMutable() const {
        if (!m_isMutable) {
            throw std::runtime_error("Cannot modify an immutable buffer");
        }
    }




    // Templated class for mutable buffer objects
    template <typename T>
    inline BufferObject<T>::BufferObject(BufferTarget target) : AbstractBufferObject(target) {}

    template <typename T>
    inline void BufferObject<T>::allocate(GLsizeiptr size, BufferUsage usage) {
        allocateBuffer(size * sizeof(T), nullptr, usage);
        setElements(size);
        setType(sizeof(T));
    }

    template <typename T>
    inline void BufferObject<T>::allocate(GLsizeiptr size, T* data, BufferUsage usage) {
        allocateBuffer(size * sizeof(T), data, usage);
        setElements(size);
        setType(sizeof(T));
    }

    template <typename T>
    inline void BufferObject<T>::allocate(std::vector<T> data, BufferUsage usage) {
        allocateBuffer(data.size() * sizeof(T), data.data(), usage);
        setElements(data.size());
        setType(sizeof(T));
    }

    template <typename T>
    inline void BufferObject<T>::write(const std::vector<T>& data) {
        writeBuffer(data.size() * sizeof(T), data.data());
    }

    template <typename T>
    inline void BufferObject<T>::write(const T* data, GLsizeiptr size) {
        writeBuffer(size * sizeof(T), data);
    }

    template <typename T>
    inline void BufferObject<T>::clear() {
        clearBuffer();
    }

    template <typename T>
    inline void BufferObject<T>::read(std::vector<T>& data) const {
        readBuffer(data.size() * sizeof(T), data.data());
    }

    template <typename T>
    inline std::vector<T> BufferObject<T>::read() const {
        GLint bufferSize;
        glGetNamedBufferParameteriv(id(), GL_BUFFER_SIZE, &bufferSize);
        std::vector<T> data(bufferSize / sizeof(T));
        readBuffer(bufferSize, data.data());
        return data;
    }

    // Templated class for immutable buffer objects
    template <typename T>
    inline ImmutableBufferObject<T>::ImmutableBufferObject(BufferTarget target) : AbstractBufferObject(target) {}

    template <typename T>
    inline void ImmutableBufferObject<T>::allocate(GLsizeiptr size, BufferStorageFlags flags) {
        allocateImmutableBuffer(size * sizeof(T), nullptr, flags);
    }

    template <typename T>
    inline void ImmutableBufferObject<T>::allocate(GLsizeiptr size, T* data, BufferStorageFlags flags) {
        allocateImmutableBuffer(size * sizeof(T), data, flags);
    }

    template <typename T>
    inline void ImmutableBufferObject<T>::allocate(std::vector<T> data, BufferStorageFlags flags) {
        allocateImmutableBuffer(data.size() * sizeof(T), data.data(), flags);
    }

    template <typename T>
    inline void ImmutableBufferObject<T>::clear() {
        clearBuffer();
    }

    template <typename T>
    inline void ImmutableBufferObject<T>::read(std::vector<T>& data) const {
        readBuffer(data.size() * sizeof(T), data.data());
    }

    template <typename T>
    inline std::vector<T> ImmutableBufferObject<T>::read() const {
        GLint bufferSize;
        glGetNamedBufferParameteriv(id(), GL_BUFFER_SIZE, &bufferSize);
        std::vector<T> data(bufferSize / sizeof(T));
        readBuffer(bufferSize, data.data());
        return data;
    }
}