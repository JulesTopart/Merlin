#include "glpch.h"
#include "RenderBuffer.h"

namespace Merlin {
	RenderBuffer::RenderBuffer(int samples) : _samples(samples){

		// Generate a new renderbuffer object
		glGenRenderbuffers(1, &_RenderbufferID);

		// bind the renderbuffer object to the GL_RENDERBUFFER target
		glBindRenderbuffer(GL_RENDERBUFFER, _RenderbufferID);
	}

	RenderBuffer::~RenderBuffer() {
		// Delete the renderbuffer object
		glDeleteRenderbuffers(1, &_RenderbufferID);
	}

	void RenderBuffer::bind() {
		// bind the renderbuffer object to the GL_RENDERBUFFER target
		glBindRenderbuffer(GL_RENDERBUFFER, _RenderbufferID);
	}

	void RenderBuffer::unbind() {
		// Unind the renderbuffer object to the GL_RENDERBUFFER target
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void RenderBuffer::resize(GLsizei width, GLsizei height) {
		// reserve storage for the renderbuffer
		if (_samples > 0)
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, _format, width, height);
		else
			glRenderbufferStorage(GL_RENDERBUFFER, _format, width, height);
	}

	void RenderBuffer::reserve(GLsizei width, GLsizei height, GLenum format) {
		_format = format;
		// reserve storage for the renderbuffer
		if(_samples > 0)
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, _samples, format, width, height);
		else
			glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
	}
}