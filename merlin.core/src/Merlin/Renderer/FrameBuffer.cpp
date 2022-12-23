#include "glpch.h"
#include "FrameBuffer.h"

namespace Merlin::Renderer {

    GLint FrameBuffer::MaxColorAttachement = 0;

    FrameBuffer::FrameBuffer(int width, int height)
    {
        m_width = width;
        m_height = height;

        if (MaxColorAttachement == 0) {
            glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &MaxColorAttachement);
            Console::info("FrameBuffer") << "This implementation support at max " << MaxColorAttachement << " color attachement" << Console::endl;
        }
        // Generate and bind the framebuffer object
        glGenFramebuffers(1, &m_fbo);
        Bind();
    }

    FrameBuffer::~FrameBuffer(){
        glDeleteFramebuffers(1, &m_fbo);
    }

    void FrameBuffer::Bind(GLenum target){
        glBindFramebuffer(target, m_fbo);
    }

    void FrameBuffer::Unbind(){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Resize(GLsizei width, GLsizei height) {
        // Update the dimensions of the framebuffer
        m_width = width;
        m_height = height;

        // Resize each of the textures attached to the framebuffer
        for (const auto& tex : textures) {
            tex->Bind();
            tex->Resize(width, height);
        }

        // Resize the depth texture, if it exists
        if (depth_tex) {
            depth_tex->Bind();
            depth_tex->Resize(width, height);
        }

        // Resize the depth and stencil renderbuffers, if they exist
        if (depth_rbo) {
            depth_rbo->Bind();
            depth_rbo->Resize(width, height);
        }
        if (stencil_rbo) {
            stencil_rbo->Bind();
            stencil_rbo->Resize(width, height);
        }
    }


    void FrameBuffer::SetDrawBuffer(GLenum drawBuffer){
        glDrawBuffer(drawBuffer);
    }


    void FrameBuffer::Print() {
        Console::info("FrameBuffer") << "The current FrameBuffer contain : " << Console::endl;

        for (GLsizei i = 0; i < textures.size(); i++) {
            Console::info("\t:") << "GL_COLOR_ATTACHMENT" <<  i << Console::endl;
        }
        if (depth_rbo || depth_tex)  Console::info("\t:") << "GL_DEPTH_ATTACHMENT" << Console::endl;
        if (stencil_rbo)  Console::info("\t:") << "GL_STENCIL_ATTACHMENT" << Console::endl;
    }

    void FrameBuffer::SetAllDrawBuffer() {
        std::vector<GLuint> attachments;
        for (GLsizei i = 0; i < textures.size(); i++) {
            attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        if (depth_rbo || depth_tex) attachments.push_back(GL_DEPTH_ATTACHMENT);
        if(stencil_rbo) attachments.push_back(GL_STENCIL_ATTACHMENT);

        if(attachments.size() > 0) glDrawBuffers(attachments.size(), attachments.data());
    }

    void FrameBuffer::AddColorAttachment(std::shared_ptr<Texture> tex) {
        if (textures.size() == MaxColorAttachement) {
            Console::error("FrameBuffer") << "This framebuffer cannot handle more color attachment (" << textures.size() << "/" << MaxColorAttachement << ")" << Console::endl;
            return;
        }
        
        // Bind the framebuffer and the texture
        Bind();
        tex->Bind();

        textures.push_back(tex);
        // Attach the texture to the framebuffer as a color attachment
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + textures.size(), tex->GetTarget(), tex->id(), 0);

        // Check that the framebuffer is complete
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            Console::error("FrameBuffer") << "Error creating color attachement: " << status << Console::endl;
        }
        Unbind();
    }

    void FrameBuffer::SetDepthAttachment(std::shared_ptr<RenderBuffer> rbo) {
        // Bind the framebuffer
        Bind();
        depth_rbo = rbo;
        // Attach the renderbuffer to the framebuffer as a depth attachment
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo->id());

        // Check that the framebuffer is complete
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            Console::error("FrameBuffer") << "Error creating Depth attachement: " << status << Console::endl;
        }
        Unbind();
    }

    void FrameBuffer::SetDepthAttachment(std::shared_ptr<Texture> tex) {
        // Bind the framebuffer and the texture
        Bind();

        depth_tex = tex;
        // Attach the texture to the framebuffer as a color attachment
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex->GetTarget(), tex->id(), 0);

        // Check that the framebuffer is complete
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            Console::error("FrameBuffer") << "Error creating Depth attachement: " << status << Console::endl;
        }
        Unbind();
    }

    void FrameBuffer::SetStencilAttachment(std::shared_ptr<RenderBuffer> rbo) {
        // Bind the framebuffer
        Bind();
        depth_rbo = rbo;
        // Attach the renderbuffer to the framebuffer as a depth attachment
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo->id());

        // Check that the framebuffer is complete
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            Console::error("FrameBuffer") << "Error creating Stencil attachement: " << status << Console::endl;
        }
        Unbind();
    }

    std::shared_ptr<RenderBuffer> FrameBuffer::CreateRenderBufferAttachment(GLenum format, GLuint samples){
        std::shared_ptr<RenderBuffer> rbo = std::make_shared<RenderBuffer>(samples);
        rbo->AllocateStorage(m_width, m_height, format);
        return rbo;
    }

    std::shared_ptr<Texture> FrameBuffer::CreateTextureAttachment(GLenum format, GLuint samples) {
        std::shared_ptr<Texture> tex = std::make_shared<Texture>(samples);
        tex->LoadFromData(NULL, m_width, m_height, Texture::Type::DIFFUSE, format);

        if (samples == 0) {
            tex->SetInterpolationMode(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
            tex->SetRepeatMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
        }
        return tex;
    }

    std::shared_ptr<FrameBuffer> FrameBuffer::Create(int width, int height) {
        return std::make_shared<FrameBuffer>(width, height);
    }

    std::shared_ptr<Texture> FrameBuffer::GetColorAttachment(GLsizei id) {
        return textures[id];
    }

}
