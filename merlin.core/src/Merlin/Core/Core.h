// Mostly from Hazel
#pragma once

#include <memory>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef GLCORE_DEBUG
	#define GLCORE_ENABLE_ASSERTS
#endif

#ifdef GLCORE_ENABLE_ASSERTS
	#define GLCORE_ASSERT(x, s, m) { if(!(x)) { LOG_ERROR(s) << m << LOG_ENDL __debugbreak(); } }
#else
	#define GLCORE_ASSERT(x, s, m)
#endif

#define BIT(x) (1 << x)

#define GLCORE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)