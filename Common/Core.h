#pragma once

#ifdef INTERNAL_ENGINE_BUILD_PLATFORM
	#ifdef INTERNAL_ENGINE_STATIC_BUILD
		#define INTERNAL_API __declspec(dllexport)
	#else
		#define INTERNAL_API __declspec(dllimport)
	#endif
#endif