#pragma once

#ifdef MRLN_PLATFORM_WINDOWS
	#ifdef MRLN_BUILD_DLL
		#define MERLIN_API __declspec(dllexport)
	#else
		#define MERLIN_API __declspec(dllimport)
	#endif // ML_BUILD_DLL
#else
	#error Merlin Engine only supports Windows 64x
#endif // ML_PLATFWORM_WINDOWS
