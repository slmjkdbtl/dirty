// wengwengweng

#ifndef D_PLAT_H
#define D_PLAT_H

#if defined(__APPLE__)
	#include <TargetConditionals.h>
	#if TARGET_OS_OSX
		#define D_MACOS
	#elif TARGET_OS_IOS
		#define D_IOS
	#endif
#elif defined(__EMSCRIPTEN__)
	#define D_WEB
#elif defined(_WIN32) || defined(_WIN64)
	#define D_WINDOWS
#elif defined(__ANDROID__)
	#define D_ANDROID
#elif defined(__linux__) || defined(__unix__)
	#define D_LINUX
#endif

#endif
