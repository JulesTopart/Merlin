#pragma once

#include "glpch.h"
#include "Core.h"

#define GLLOG() Console::LogGLError()
#define GLDEBUG(x) Console::ClearGLError(); x; Console::CheckGLError(__FILENAME__, __LINE__);
#define GL_(x) GLDEBUG(x)

namespace Merlin {

	enum class ConsoleLevel {
		_TRACE,
		_INFO,
		_WARNING,
		_ERROR,
		_SUCCESS,
		_CRITICAL
	};

	class ConsoleStream {
	public:

		ConsoleStream(ConsoleLevel level, std::string origin = "");

		ConsoleStream operator<<(char i);
		ConsoleStream operator<<(signed short i);
		ConsoleStream operator<<(unsigned short i);
		ConsoleStream operator<<(signed int i);
		ConsoleStream operator<<(unsigned int i);
		ConsoleStream operator<<(signed long i);
		ConsoleStream operator<<(unsigned long i);
		ConsoleStream operator<<(float i);
		ConsoleStream operator<<(double i);
		ConsoleStream operator<<(const std::string& i);
		ConsoleStream operator<<(const char* i);
		ConsoleStream operator<<(const void* i);

		ConsoleLevel level() const { return _level; }
		std::string font() const { return getFont(_level); }

		static std::string endFont() { return "\033[0m"; };
		static std::string getHeader(ConsoleLevel lv);
		static std::string getFont(ConsoleLevel lv);

	private:
		ConsoleLevel _level;
	};

	class Console {
	public:

		static std::string endl;

		static ConsoleStream trace(std::string origin = "");
		static ConsoleStream info(std::string origin = "");
		static ConsoleStream warn(std::string origin = "");
		static ConsoleStream error(std::string origin = "");
		static ConsoleStream success(std::string origin = "");
		static ConsoleStream critical(std::string origin = "");

		static void ClearGLError();
		static void LogGLError();
		static void CheckGLError(std::string file, int line);

		inline static ConsoleLevel GetLevel() { return _level; };
		inline static void  SetLevel(ConsoleLevel l) { _level = l; };
		static void write(std::string content);

	private:
		static ConsoleLevel _level;
		Console() {} //Disabling constructor
	};
}
