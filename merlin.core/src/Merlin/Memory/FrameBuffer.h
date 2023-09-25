#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Memory/Texture.h"
#include "Merlin/Memory/TextureMultisampled.h"
#include "Merlin/Memory/RenderBuffer.h"

using namespace Merlin::Graphics;

namespace Merlin::Memory {

    class FrameBuffer{
    public:
        FrameBuffer(int width, int height);
        ~FrameBuffer();

        void Bind(GLenum target = GL_FRAMEBUFFER);
        void Unbind();

        void Resize(GLsizei width, GLsizei height);
        void Print() const;

        inline GLuint const id() { return _FrameBufferID; }

        std::shared_ptr<TextureBase> GetColorAttachment(GLsizei id);
        void AddColorAttachment(std::shared_ptr<TextureBase> tex);
        void AddDepthStencilAttachment(std::shared_ptr<RenderBuffer> rbo);

        std::shared_ptr<TextureBase> CreateTextureAttachment(GLenum format, GLuint samples = 0);
        std::shared_ptr<RenderBuffer> CreateRenderBufferAttachment(GLenum format, GLuint samples = 0);

        static std::shared_ptr<FrameBuffer> Create(int width, int height);
        
    private:

        void CheckErrors(std::string prefix) const;

        std::vector<std::shared_ptr<TextureBase>> textures;
        std::shared_ptr<RenderBuffer> depth_stencil_rbo;

        static GLint MaxColorAttachement;

        int _width = 0;
        int _height = 0;

        GLuint _FrameBufferID;
    };

    typedef FrameBuffer FBO;
}
