#pragma once

#include <glad/glad.h>

#include "../Core/Log.h"

namespace Merlin::Utils {

	void EnableGLDebugging();
	void OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

}