#pragma once

#ifdef _WIN64
#define SERVLIBCALL /*__fastcall*/
#else
#define SERVLIBCALL __fastcall
#endif

#ifndef ALWAYS_INLINE
	#if defined(_MSC_VER)
		#define ALWAYS_INLINE __forceinline
	#elif defined(__GNUC__) || defined(__clang__)
		#define ALWAYS_INLINE inline __attribute__((always_inline))
	#else
		#define ALWAYS_INLINE inline
	#endif
#endif

class servlib {

};

