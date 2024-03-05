#pragma once
#include "Merlin/Core/Core.h"

#include "../Core/Log.h"

namespace Merlin {

	void enableGLDebugging();
	void openGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

}