#include "glpch.h"
#include "FrameBuffer.h"

namespace Merlin::Renderer {

    GLint FrameBuffer::MaxColorAttachement = 0;

    FrameBuffer::FrameBuffer(int width, int height)
    {
        _width = width;
        _height = height;

        if (MaxColorAttachement == 0) {
            glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &MaxColorAttachement);
            Console::info("FrameBuffer") << "This implementation support at max " << MaxColorAttachement << " color attachement" << Console::endl;
        }
        // Generate and bind the framebuffer object
        glGenFramebuffers(1, &_FrameBufferID);
    }

    FrameBuffer::~FrameBuffer(){
        glDeleteFramebuffers(1, &_FrameBufferID);
    }

    void FrameBuffer::Bind(GLenum target){
        glBindFramebuffer(target, _FrameBufferID);
    }

    void FrameBuffer::Unbind(){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Resize(GLsizei width, GLsizei height) {
        // Update the dimensions of the framebuffer
        _width = width;
        _height = height;

        // Resize each of the textures attached to the framebuffer
        for (const auto& tex : textures) {
            tex->Bind();
            tex->Resize(width, height);
        }

        // Resize the depth texture, if it exists
        if (depth_stencil_rbo) {
            depth_stencil_rbo->Bind();
            depth_stencil_rbo->Resize(width, height);
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
        if (depth_stencil_rbo)  Console::info("\t:") << "GL_DEPTH_STENCIL_ATTACHMENT" << Console::endl;
    }

    void FrameBuffer::SetAllDrawBuffer() {
        std::vector<GLenum> attachments;
        for (GLsizei i = 0; i < textures.size(); i++) {
            attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        if (depth_stencil_rbo) attachments.push_back(GL_DEPTH_STENCIL_ATTACHMENT);
        if(attachments.size() > 0) glDrawBuffers(attachments.size(), attachments.data());
    }

    void FrameBuffer::AddColorAttachment(std::shared_ptr<Texture> tex) {
        if (textures.size() == MaxColorAttachement) {
            Console::error("FrameBuffer") << "This framebuffer cannot handle more color attachment (" << textures.size() << "/" << MaxColorAttachement << ")" << Console::endl;
            return;
        }

        textures.push_back(tex);
        // Attach the texture to the framebuffer as a color attachment
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + textures.size()-1, tex->GetTarget(), tex->id(), 0);

        // Check that the framebuffer is complete
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            Console::error("FrameBuffer") << "Error creating color attachement: " << status << Console::endl;
        }
    }

    void FrameBuffer::AddDepthStencilAttachment(std::shared_ptr<RenderBuffer> rbo) {
        // Bind the framebuffer
        Bind();
        depth_stencil_rbo = rbo;
        // Attach the renderbuffer to the framebuffer as a depth attachment
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo->id());

        // Check that the framebuffer is complete
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            Console::error("FrameBuffer") << "Error creating Depth attachement: " << status << Console::endl;
        }
        Unbind();
    }

    std::shared_ptr<RenderBuffer> FrameBuffer::CreateRenderBufferAttachment(GLenum format, GLuint samples){
        std::shared_ptr<RenderBuffer> rbo = std::make_shared<RenderBuffer>(samples);
        rbo->AllocateStorage(_width, _height, format);
        return rbo;
    }

    std::shared_ptr<Texture> FrameBuffer::CreateTextureAttachment(GLenum format, GLuint samples) {
        
        // Create Framebuffer Texture
        std::shared_ptr<Texture> tex;
        tex = std::make_shared<Texture>(samples);
        tex->Bind();
        tex->Allocate(_width, _height, GL_RGB);

        if (samples == 0) {
            tex->SetInterpolationMode(GL_NEAREST, GL_NEAREST);
            tex->SetRepeatMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
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
