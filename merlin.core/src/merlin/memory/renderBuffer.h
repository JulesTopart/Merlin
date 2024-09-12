#pragma once
#include "merlin/core/core.h"

namespace Merlin {
    class RenderBuffer
    {
    public:
        RenderBuffer(int samples = 0);
        ~RenderBuffer();

        void bind();
        void unbind();
        void resize(GLsizei width, GLsizei height);
        
        void reserve(GLsizei width, GLsizei height, GLenum internalFormat);

        inline GLuint id() const { return _RenderbufferID; }


    private:
        GLuint _RenderbufferID;
        GLuint _samples;
        GLenum _format;

    };
    typedef RenderBuffer RBO;
}

