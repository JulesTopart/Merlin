#include "glpch.h"
#include "OpenGLDebug.h"

namespace Merlin::Utils {

	void OpenGLLogMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			{
				LOG_ERROR("OpenGL Debug") << message << Console::endl;
				//GLCORE_ASSERT(false, "OpenGL", "GL_DEBUG_SEVERITY_HIGH");
			}
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
				LOG_WARN("OpenGL Debug") << message << Console::endl;
			break;
		case GL_DEBUG_SEVERITY_LOW:
				LOG_INFO("OpenGL Debug") << message << Console::endl;
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
				//LOG_TRACE("OpenGL Debug") << message << Console::endl;
			break;
		default:
			;// LOG_TRACE("OpenGL Debug") << message << Console::endl;
			break;
		}
	}

	void EnableGLDebugging()
	{
		glDebugMessageCallback(OpenGLLogMessage, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

}