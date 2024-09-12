#include "glpch.h"
#include "frameBuffer.h"


namespace Merlin {

    GLint FrameBuffer::MaxColorAttachement = 0;

    FrameBuffer::FrameBuffer(int width, int height)
    {
        _width = width;
        _height = height;

        if (MaxColorAttachement == 0) {
            glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &MaxColorAttachement);
            Console::info("FrameBuffer") << "This implementation support at max " << MaxColorAttachement << " color attachement" << Console::endl;
        }

        if (!m_texRenderer) {
            m_texRenderer = createShared<ScreenQuadRenderer>();
        }

        // Generate and bind the framebuffer object
        glGenFramebuffers(1, &_FrameBufferID);
    }

    FrameBuffer::~FrameBuffer(){
        glDeleteFramebuffers(1, &_FrameBufferID);
    }

    void FrameBuffer::bind(GLenum target){
        glBindFramebuffer(target, _FrameBufferID);
    }

    void FrameBuffer::unbind(){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::resize(GLsizei width, GLsizei height) {
        // Update the dimensions of the framebuffer
        _width = width;
        _height = height;
    }

    void FrameBuffer::print() const {
        Console::info("FrameBuffer") << "The current FrameBuffer contain : " << Console::endl;

        for (GLsizei i = 0; i < m_attatchments.size(); i++) {
            Console::info("\t:") << "GL_COLOR_ATTACHMENT" <<  i << Console::endl;
        }
    }

    void FrameBuffer::checkErrors(std::string e) const {
        // Check that the framebuffer is complete
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            Console::error("FrameBuffer") << e << " " << status << Console::endl;
        }
    }

    void FrameBuffer::attachColorTexture(Shared<TextureBase> tex) {
        if (m_attatchments.size() == MaxColorAttachement) {
            Console::error("FrameBuffer") << "This framebuffer cannot handle more color attachment (" << m_attatchments.size() << "/" << MaxColorAttachement << ")" << Console::endl;
            return;
        }
        // attach the texture to the framebuffer as a color attachment
        tex->bind();
        m_attatchments.push_back(GL_COLOR_ATTACHMENT0 + m_attatchments.size() - 1);
        glFramebufferTexture2D(GL_FRAMEBUFFER, m_attatchments.back(), tex->getTarget(), tex->id(), 0);
        checkErrors("Error creating GL_COLOR_ATTACHMENT");
    }

    void FrameBuffer::attachDepthStencilRBO(Shared<RenderBuffer> rbo) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo->id());
        // Check that the framebuffer is complete
        checkErrors("Error creating GL_DEPTH_STENCIL_ATTACHMENT");
    }

    void FrameBuffer::attachDepthStencilTexture(Shared<TextureBase> tex) {
        //depth_stencil_texture->bind();
        if(tex->textureClass() == TextureClass::TEXTURE2D || tex->textureClass() == TextureClass::TEXTURE2D_MULTISAMPLED)
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, tex->getTarget(), tex->id(), 0);
        else if(tex->textureClass() == TextureClass::CUBE_MAP)
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, tex->id(), 0);

        // Check that the framebuffer is complete
        checkErrors("Error creating GL_DEPTH_STENCIL_ATTACHMENT");
    }
    /*
    void FrameBuffer::setDepthAttachment(Shared<RenderBuffer> rbo) {
        if (depth_rbo || depth_texture) {
            Console::error("FrameBuffer") << "GL_DEPTH_ATTACHMENT already set !" << Console::endl;
            return;
        }

        depth_stencil_rbo = rbo;
        // attach the renderbuffer to the framebuffer as a depth attachment
        rbo->bind();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo->id());

        // Check that the framebuffer is complete
        checkErrors("Error creating GL_DEPTH_ATTACHMENT");
    }

    void FrameBuffer::setDepthAttachment(Shared<TextureBase> tex) {
        if (depth_texture) {
            Console::error("FrameBuffer") << "GL_DEPTH_ATTACHMENT already set as a Texture !" << Console::endl;
            return;
        }
        // create Framebuffer Texture
        depth_texture = tex;
        depth_texture->bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture->getTarget(), depth_texture->id(), 0);

        // Check that the framebuffer is complete
        checkErrors("Error creating GL_DEPTH_ATTACHMENT");
    }
    */
    void FrameBuffer::renderAttachement(GLuint id) {
        bind(GL_READ_FRAMEBUFFER);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + id);
        //fbo->bind(GL_DRAW_FRAMEBUFFER);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        // Copy the multisampled framebuffer to the non-multisampled framebuffer, applying multisample resolve filters as needed
        glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        bind();
    }

    /*
    std::shared_ptr<RenderBuffer> FrameBuffer::createRenderBufferAttachment(GLenum format, GLuint samples){
        std::shared_ptr<RenderBuffer> rbo = std::make_shared<RenderBuffer>(samples);
        rbo->bind();
        rbo->reserve(_width, _height, format);
        return rbo;
    }

    
    std::shared_ptr<TextureBase> FrameBuffer::createTextureAttachment(GLenum format, GLuint samples) {
        
        // create Framebuffer Texture
        std::shared_ptr<TextureBase> tex = nullptr;
        if (samples > 0) {
            tex = std::make_shared<TextureMultisampled2D>(samples, TextureType::ALBEDO);
            tex->reserve(_width, _height, format, format);
        }
        else {
            tex = std::make_shared<Texture2D>();
            tex->bind();
            if (const auto tx = std::dynamic_pointer_cast<Texture2D>(tex)) {
                tx->setInterpolationMode(GL_NEAREST, GL_NEAREST);
                tx->setRepeatMode(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
            }
            tex->reserve(_width, _height, 8);
        }
        return tex;
    }*/

    std::shared_ptr<FrameBuffer> FrameBuffer::create(int width, int height) {
        return std::make_shared<FrameBuffer>(width, height);
    }


    void FrameBuffer::setDrawBuffer(std::vector<unsigned int> buffers) {

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
