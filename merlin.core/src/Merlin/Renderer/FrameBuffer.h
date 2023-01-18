#pragma once
#include "Merlin/Core/Core.h"

#include <vector>
#include "Texture.h"
#include "RenderBuffer.h"

namespace Merlin::Renderer {

    class FrameBuffer{
    public:
        FrameBuffer(int width, int height);
        ~FrameBuffer();

        void Bind(GLenum target = GL_FRAMEBUFFER);
        void Unbind();

        void Resize(GLsizei width, GLsizei height);
        void Print() const;

        inline GLuint const id() { return _FrameBufferID; }

        std::shared_ptr<Texture> GetColorAttachment(GLsizei id);
        void AddColorAttachment(std::shared_ptr<Texture> tex);
        void AddDepthStencilAttachment(std::shared_ptr<RenderBuffer> rbo);

        std::shared_ptr<Texture> CreateTextureAttachment(GLenum format, GLuint samples = 0);
        std::shared_ptr<RenderBuffer> CreateRenderBufferAttachment(GLenum format, GLuint samples = 0);

        static std::shared_ptr<FrameBuffer> Create(int width, int height);
        
    private:

        void CheckErrors(std::string prefix) const;

        std::vector<std::shared_ptr<Texture>> textures;
        std::shared_ptr<RenderBuffer> depth_stencil_rbo;

        static GLint MaxColorAttachement;

        int _width = 0;
        int _height = 0;

        GLuint _FrameBufferID;
    };

    typedef FrameBuffer FBO;
}
