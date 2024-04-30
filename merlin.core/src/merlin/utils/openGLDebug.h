#pragma once
#include "merlin/core/core.h"

#include "merlin/core/log.h"

namespace Merlin {

	void enableGLDebugging();
	void openGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

}