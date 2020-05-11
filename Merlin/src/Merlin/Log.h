#pragma once
#include "Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Merlin {

	class MERLIN_API Log {
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define ML_CORE_TRACE(...)    ::Hazel::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ML_CORE_INFO(...)     ::Hazel::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ML_CORE_WARN(...)     ::Hazel::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ML_CORE_ERROR(...)    ::Hazel::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ML_CORE_CRITICAL(...) ::Hazel::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define ML_TRACE(...)         ::Hazel::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ML_INFO(...)          ::Hazel::Log::GetClientLogger()->info(__VA_ARGS__)
#define ML_WARN(...)          ::Hazel::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ML_ERROR(...)         ::Hazel::Log::GetClientLogger()->error(__VA_ARGS__)
#define ML_CRITICAL(...)      ::Hazel::Log::GetClientLogger()->critical(__VA_ARGS__)