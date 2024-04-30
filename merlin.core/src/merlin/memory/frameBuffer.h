#pragma once
#include "merlin/core/core.h"

#include "merlin/memory/texture.h"
#include "merlin/memory/textureMultisampled.h"
#include "merlin/memory/renderBuffer.h"

namespace Merlin {

    class FrameBuffer{
    public:
        FrameBuffer(int width, int height);
        ~FrameBuffer();

        void bind(GLenum target = GL_FRAMEBUFFER);
        void unbind();

        void resize(GLsizei width, GLsizei height);
        void print() const;

        inline GLuint const id() { return _FrameBufferID; }

        void setDrawBuffer(std::vector<unsigned int> buffers = std::vector<unsigned int>());
        std::shared_ptr<TextureBase> getColorAttachment(GLsizei id);
        std::shared_ptr<TextureBase> getDepthAttachement();
        void addColorAttachment(Shared<TextureBase> tex);
        void addDepthStencilAttachment(Shared<RenderBuffer> rbo);
        void addDepthStencilAttachment(Shared<TextureBase> tex);

        void renderAttachement(GLuint id);

        std::shared_ptr<TextureBase> createTextureAttachment(GLenum format, GLuint samples = 0);
        std::shared_ptr<RenderBuffer> createRenderBufferAttachment(GLenum format, GLuint samples = 0);

        static std::shared_ptr<FrameBuffer> create(int width, int height);
        
    private:

        void checkErrors(std::string prefix) const;

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
