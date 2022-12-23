#pragma once
#include <vector>
#include <glad/glad.h>

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
        void SetDrawBuffer(GLenum drawBuffer);
        void SetAllDrawBuffer();
        void Print();

        static std::shared_ptr<FrameBuffer> Create(int width, int height);

        std::shared_ptr<Texture> GetColorAttachment(GLsizei id);
        void AddColorAttachment(std::shared_ptr<Texture> tex);
        void SetDepthAttachment(std::shared_ptr<Texture> tex);
        void SetDepthAttachment(std::shared_ptr<RenderBuffer>  rbo);
        void SetStencilAttachment(std::shared_ptr<RenderBuffer>  rbo);

        std::shared_ptr<Texture> CreateTextureAttachment(GLenum format, GLuint samples);
        std::shared_ptr<RenderBuffer> CreateRenderBufferAttachment(GLenum format, GLuint samples);
        
    private:
        std::vector<std::shared_ptr<Texture>> textures;
        std::shared_ptr<RenderBuffer> depth_rbo;
        std::shared_ptr<Texture> depth_tex;
        std::shared_ptr<RenderBuffer> stencil_rbo;

        static GLint MaxColorAttachement;

        int m_width = 0;
        int m_height = 0;

        GLuint m_fbo;
    };
}
