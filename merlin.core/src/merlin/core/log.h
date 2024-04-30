#pragma once
#include "console.h"

// Client log macros

#define LOG_ENDL			   Merlin::Console::endl;
#define LOG_TRACE(...)         Merlin::Console::trace(__VA_ARGS__)
#define LOG_INFO(...)          Merlin::Console::info(__VA_ARGS__)
#define LOG_OK(...)			   Merlin::Console::success(__VA_ARGS__)
#define LOG_WARN(...)          Merlin::Console::warn(__VA_ARGS__)
#define LOG_ERROR(...)         Merlin::Console::error(__VA_ARGS__)
#define LOG_CRITICAL(...)      Merlin::Console::critical(__VA_ARGS__)
