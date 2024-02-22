#pragma once
#include "Merlin/Core/Core.h"

#include "Merlin/Memory/Texture.h"
#include "Merlin/Memory/TextureMultisampled.h"
#include "Merlin/Memory/RenderBuffer.h"

namespace Merlin {

    class FrameBuffer{
    public:
        FrameBuffer(int width, int height);
        ~FrameBuffer();

        void Bind(GLenum target = GL_FRAMEBUFFER);
        void Unbind();

        void Resize(GLsizei width, GLsizei height);
        void Print() const;

        inline GLuint const id() { return _FrameBufferID; }

        void SetDrawBuffer(std::vector<unsigned int> buffers = std::vector<unsigned int>());
        std::shared_ptr<TextureBase> GetColorAttachment(GLsizei id);
        std::shared_ptr<TextureBase> GetDepthAttachement();
        void AddColorAttachment(Shared<TextureBase> tex);
        void AddDepthStencilAttachment(Shared<RenderBuffer> rbo);
        void AddDepthStencilAttachment(Shared<TextureBase> tex);

        void RenderAttachement(GLuint id);

        std::shared_ptr<TextureBase> CreateTextureAttachment(GLenum format, GLuint samples = 0);
        std::shared_ptr<RenderBuffer> CreateRenderBufferAttachment(GLenum format, GLuint samples = 0);

        static std::shared_ptr<FrameBuffer> Create(int width, int height);
        
    private:

        void CheckErrors(std::string prefix) const;

        std::vector<GLuint> m_attatchments;
        std::vector<std::shared_ptr<TextureBase>> textures;

        std::shared_ptr<RenderBuffer> depth_stencil_rbo;
        std::shared_ptr<TextureBase> depth_texture;

        static GLint MaxColorAttachement;

        int _width = 0;
        int _height = 0;

        GLuint _FrameBufferID;
    };

    typedef FrameBuffer FBO;
    typedef Shared<FBO> FBO_Ptr;
}
