#include "glpch.h"
#include "FrameBuffer.h"

namespace Merlin::Memory {

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
            tex->Unbind();
        }

        // Resize the depth texture, if it exists
        if (depth_stencil_rbo) {
            depth_stencil_rbo->Bind();
            depth_stencil_rbo->Resize(width, height);
            depth_stencil_rbo->Unbind();
        }
    }

    void FrameBuffer::Print() const {
        Console::info("FrameBuffer") << "The current FrameBuffer contain : " << Console::endl;

        for (GLsizei i = 0; i < textures.size(); i++) {
            Console::info("\t:") << "GL_COLOR_ATTACHMENT" <<  i << Console::endl;
        }
        if (depth_stencil_rbo)  Console::info("\t:") << "GL_DEPTH_STENCIL_ATTACHMENT" << Console::endl;
    }

    void FrameBuffer::CheckErrors(std::string e) const {
        // Check that the framebuffer is complete
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            Console::error("FrameBuffer") << e << status << Console::endl;
        }
    }

    void FrameBuffer::AddColorAttachment(Shared<TextureBase> tex) {
        if (textures.size() == MaxColorAttachement) {
            Console::error("FrameBuffer") << "This framebuffer cannot handle more color attachment (" << textures.size() << "/" << MaxColorAttachement << ")" << Console::endl;
            return;
        }

        textures.push_back(tex);
        // Attach the texture to the framebuffer as a color attachment
        tex->Bind();
        m_attatchments.push_back(GL_COLOR_ATTACHMENT0 + textures.size() - 1);
        glFramebufferTexture2D(GL_FRAMEBUFFER, m_attatchments.back(), tex->GetTarget(), tex->id(), 0);
        CheckErrors("Error creating GL_COLOR_ATTACHMENT");
    }

    void FrameBuffer::AddDepthStencilAttachment(Shared<RenderBuffer> rbo) {
        if (depth_stencil_rbo) {
            Console::error("FrameBuffer") << "GL_DEPTH_STENCIL_ATTACHMENT already set as a RBO !" << Console::endl;
            return;
        }

        if (depth_texture) {
            Console::error("FrameBuffer") << "GL_DEPTH_ATTACHMENT already set as a Texture !" << Console::endl;
            return;
        }

        depth_stencil_rbo = rbo;
        // Attach the renderbuffer to the framebuffer as a depth attachment
        rbo->Bind();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo->id());



        // Check that the framebuffer is complete
        CheckErrors("Error creating GL_DEPTH_STENCIL_ATTACHMENT");
    }

    void FrameBuffer::AddDepthStencilAttachment(Shared<TextureBase> tex) {
        if (depth_stencil_rbo) {
            Console::error("FrameBuffer") << "GL_DEPTH_STENCIL_ATTACHMENT already set as a RBO !" << Console::endl;
            return;
        }

        if (depth_texture) {
            Console::error("FrameBuffer") << "GL_DEPTH_ATTACHMENT already set as a Texture !" << Console::endl;
            return;
        }
        // Create Framebuffer Texture
        depth_texture = tex;
        depth_texture->Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture->GetTarget(), depth_texture->id(), 0);

        // Check that the framebuffer is complete
        CheckErrors("Error creating GL_DEPTH_STENCIL_ATTACHMENT");
    }

    void FrameBuffer::RenderAttachement(GLuint id) {
        Bind(GL_READ_FRAMEBUFFER);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + id);
        //fbo->Bind(GL_DRAW_FRAMEBUFFER);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        // Copy the multisampled framebuffer to the non-multisampled framebuffer, applying multisample resolve filters as needed
        glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        Bind();
    }

    std::shared_ptr<RenderBuffer> FrameBuffer::CreateRenderBufferAttachment(GLenum format, GLuint samples){
        std::shared_ptr<RenderBuffer> rbo = std::make_shared<RenderBuffer>(samples);
        rbo->Bind();
        rbo->AllocateStorage(_width, _height, format);
        return rbo;
    }

    std::shared_ptr<TextureBase> FrameBuffer::CreateTextureAttachment(GLenum format, GLuint samples) {
        
        // Create Framebuffer Texture
        std::shared_ptr<TextureBase> tex;
        if (samples > 0) {
            tex = std::make_shared<TextureMultisampled>(TextureType::COLOR, samples);
        }
        else {
            tex = std::make_shared<Texture>();
            if (const auto tx = std::dynamic_pointer_cast<Texture>(tex)) {
                tx->SetInterpolationMode(GL_LINEAR, GL_LINEAR);
                tx->SetRepeatMode(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
            }
        }

        tex->Bind();
        tex->Allocate(_width, _height, format);

        return tex;
    }

    std::shared_ptr<FrameBuffer> FrameBuffer::Create(int width, int height) {
        return std::make_shared<FrameBuffer>(width, height);
    }

    std::shared_ptr<TextureBase> FrameBuffer::GetColorAttachment(GLsizei id) {
        return textures[id];
    }

    std::shared_ptr<TextureBase> FrameBuffer::GetDepthAttachement() {
        return depth_texture;
    }

    void FrameBuffer::SetDrawBuffer(std::vector<unsigned int> buffers) {

        if (buffers.size() == 0) {
            buffers = m_attatchments;
        }
        else {
            for (unsigned int i = 0; i < buffers.size(); i++) {
                if (buffers[i] < 0 || buffers[i] >= m_attatchments.size()) Console::error("FrameBuffer") << "Given attachement not found in current fbo : GL_COLOR_ATTACHMENT" << buffers[i] << Console::endl;
                else buffers[i] = m_attatchments[buffers[i]];
            }
        }
        glDrawBuffers(buffers.size(), buffers.data());
    }

}
