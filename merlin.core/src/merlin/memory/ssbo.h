#pragma once

#include <vector>
#include <string>
#include <memory>

#include "merlin/core/core.h"
#include "merlin/shaders/shaderBase.h"
#include "merlin/memory/bufferObject.h"

namespace Merlin {

    // Mutable Shader Storage Buffer Object
    template<class T>
    class ShaderStorageBuffer : public BufferObject<T> {
    public:
        ShaderStorageBuffer();
        ShaderStorageBuffer(const std::string& name);
        ShaderStorageBuffer(const std::string& name, GLsizeiptr size, T* data, BufferUsage usage = BufferUsage::StaticDraw);
        ShaderStorageBuffer(const std::string& name, GLsizeiptr count, BufferUsage usage = BufferUsage::StaticDraw);
        ShaderStorageBuffer(const std::string& name, std::vector<T>data, BufferUsage usage = BufferUsage::StaticDraw);

        virtual ~ShaderStorageBuffer();


        void print() const;

        static void copy(std::shared_ptr<ShaderStorageBuffer> origin, std::shared_ptr<ShaderStorageBuffer> target, GLsizeiptr size);
        static std::shared_ptr<ShaderStorageBuffer<T>> create(const std::string& name);
        static std::shared_ptr<ShaderStorageBuffer<T>> create(const std::string& name, GLsizeiptr count, BufferUsage usage = BufferUsage::StaticDraw);
        static std::shared_ptr<ShaderStorageBuffer<T>> create(const std::string& name, GLsizeiptr count, T* data, BufferUsage usage = BufferUsage::StaticDraw);
        static std::shared_ptr<ShaderStorageBuffer<T>> create(const std::string& name, std::vector<T>data, BufferUsage usage = BufferUsage::StaticDraw);
    };

    template<class T>
    using SSBO = ShaderStorageBuffer<T>; // Shorter alias
    template<class T>
    using SSBO_Ptr = std::shared_ptr<ShaderStorageBuffer<T>>; // Shorter alias

    // Implementation
    template <class T>
    inline ShaderStorageBuffer<T>::ShaderStorageBuffer() : BufferObject<T>(BufferTarget::Shader_Storage_Buffer) {}

    template <class T>
    inline ShaderStorageBuffer<T>::ShaderStorageBuffer(const std::string& name) : BufferObject<T>(BufferTarget::Shader_Storage_Buffer) { this->rename(name); }

    template <class T>
    inline ShaderStorageBuffer<T>::ShaderStorageBuffer(const std::string& name, GLsizeiptr size, T* data, BufferUsage usage)
        : BufferObject<T>(BufferTarget::Shader_Storage_Buffer) {
        this->allocate(size, data, usage );
        this->rename(name);
    }

    template <class T>
    inline ShaderStorageBuffer<T>::ShaderStorageBuffer(const std::string& name, GLsizeiptr size, BufferUsage usage)
        : BufferObject<T>(BufferTarget::Shader_Storage_Buffer) {
        this->allocate(size, usage);
        this->rename(name);
    }

    template <class T>
    inline ShaderStorageBuffer<T>::ShaderStorageBuffer(const std::string& name, std::vector<T> data, BufferUsage usage)
        : BufferObject<T>(BufferTarget::Shader_Storage_Buffer) {
        this->allocate( data, usage);
        this->rename(name);
    }

    template <class T>
    inline ShaderStorageBuffer<T>::~ShaderStorageBuffer() {}

    template <class T>
    inline void ShaderStorageBuffer<T>::copy(std::shared_ptr<ShaderStorageBuffer> origin, std::shared_ptr<ShaderStorageBuffer> target, GLsizeiptr size) {
        glBindBuffer(GL_COPY_READ_BUFFER, origin->getBufferId());
        glBindBuffer(GL_COPY_WRITE_BUFFER, target->getBufferId());

        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);
    }

    template <class T>
    inline std::shared_ptr<ShaderStorageBuffer<T>> ShaderStorageBuffer<T>::create(const std::string& name) {
        return std::make_shared<ShaderStorageBuffer>(name);
    }

    template <class T>
    inline std::shared_ptr<ShaderStorageBuffer<T>> ShaderStorageBuffer<T>::create(const std::string& name, GLsizeiptr count, BufferUsage usage) {
        return std::make_shared<ShaderStorageBuffer>(name, count, usage);
    }

    template <class T>
    inline std::shared_ptr<ShaderStorageBuffer<T>> ShaderStorageBuffer<T>::create(const std::string& name, GLsizeiptr size, T* data, BufferUsage usage) {
        return std::make_shared<ShaderStorageBuffer>(name, size, data, usage);
    }

    template <class T>
    inline std::shared_ptr<ShaderStorageBuffer<T>> ShaderStorageBuffer<T>::create(const std::string& name, std::vector<T> data, BufferUsage usage) {
        return std::make_shared<ShaderStorageBuffer>(name, data, usage);
    }

    template<typename T>
    void ShaderStorageBuffer<T>::print() const{
        //bind();
        std::vector<T> cpuBuffer = ShaderStorageBuffer<T>::read();

        Console::info("Buffer") << ShaderStorageBuffer<T>::name() << " = (" << cpuBuffer.size() << ")[";
        for (GLuint i = 0; i < std::min(int(cpuBuffer.size()), 100); ++i) {
            Console::print() << cpuBuffer[i] << ", ";
        }
        if (cpuBuffer.size() > 100) {
            Console::print() << "..., ";
            Console::print() << cpuBuffer[cpuBuffer.size() - 1];
        }
        else if (cpuBuffer.empty()) Console::print() << "empty";
        Console::print() << "]" << Console::endl << Console::endl;

    }



    // Immutable Shader Storage Buffer Object
    template<class T>
    class ImmutableShaderStorageBuffer : public ImmutableBufferObject<T> {
    public:
        ImmutableShaderStorageBuffer();
        ImmutableShaderStorageBuffer(const std::string& name, GLsizeiptr count, BufferStorageFlags flag);
        ImmutableShaderStorageBuffer(const std::string& name, GLsizeiptr count, T* data, BufferStorageFlags flag);
        ImmutableShaderStorageBuffer(const std::string& name, std::vector<T> data, BufferStorageFlags flag);

        virtual ~ImmutableShaderStorageBuffer();

        static std::shared_ptr<ImmutableShaderStorageBuffer<T>> create(const std::string& name, GLsizeiptr count, BufferStorageFlags flag);
        static std::shared_ptr<ImmutableShaderStorageBuffer<T>> create(const std::string& name, GLsizeiptr count, T* data, BufferStorageFlags flag);
        static std::shared_ptr<ImmutableShaderStorageBuffer<T>> create(const std::string& name, std::vector<T> data, BufferStorageFlags flag);
    };

    template<class T>
    using ImmutableSSBO = ImmutableShaderStorageBuffer<T>; // Shorter alias
    template<class T>
    using ImmutableSSBO_Ptr = std::shared_ptr<ImmutableShaderStorageBuffer<T>>; // Shorter alias


    // Implementation of ImmutableShaderStorageBuffer
    template <class T>
    inline ImmutableShaderStorageBuffer<T>::ImmutableShaderStorageBuffer() : ImmutableBufferObject<T>(BufferTarget::Shader_Storage_Buffer) {}

    template <class T>
    inline ImmutableShaderStorageBuffer<T>::ImmutableShaderStorageBuffer(const std::string& name, GLsizeiptr count, BufferStorageFlags flag)
        : ImmutableBufferObject<T>(BufferTarget::Shader_Storage_Buffer) {
        this->allocate(count, flag);
        this->rename(name);
    }

    template <class T>
    inline ImmutableShaderStorageBuffer<T>::ImmutableShaderStorageBuffer(const std::string& name, GLsizeiptr count, T* data, BufferStorageFlags flag)
        : ImmutableBufferObject<T>(BufferTarget::Shader_Storage_Buffer) {
        this->allocate(count, data, flag);
        this->rename(name);
    }

    template <class T>
    inline ImmutableShaderStorageBuffer<T>::ImmutableShaderStorageBuffer(const std::string& name, std::vector<T> data, BufferStorageFlags flag)
        : ImmutableBufferObject<T>(BufferTarget::Shader_Storage_Buffer) {
        this->allocate(data, flag);
        this->rename(name);
    }


    template <class T>
    inline ImmutableShaderStorageBuffer<T>::~ImmutableShaderStorageBuffer() {}

    template <class T>
    inline std::shared_ptr<ImmutableShaderStorageBuffer<T>> ImmutableShaderStorageBuffer<T>::create(const std::string& name, GLsizeiptr count, BufferStorageFlags flag) {
        return std::make_shared<ImmutableShaderStorageBuffer>(name, count, flag);
    }

    template <class T>
    inline std::shared_ptr<ImmutableShaderStorageBuffer<T>> ImmutableShaderStorageBuffer<T>::create(const std::string& name, GLsizeiptr count, T* data, BufferStorageFlags flag) {
        return std::make_shared<ImmutableShaderStorageBuffer>(name, count, data, flag);
    }

    template <class T>
    inline std::shared_ptr<ImmutableShaderStorageBuffer<T>> ImmutableShaderStorageBuffer<T>::create(const std::string& name, std::vector<T> data, BufferStorageFlags flag) {
        return std::make_shared<ImmutableShaderStorageBuffer>(name, data, flag);
    }

} // namespace Merlin

