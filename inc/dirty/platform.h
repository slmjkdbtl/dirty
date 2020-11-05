// wengwengweng

#ifndef D_PLATFORM_H
#define D_PLATFORM_H

#if defined(__APPLE__)
	#include <TargetConditionals.h>
    #if defined(TARGET_OS_IPHONE) && !TARGET_OS_IPHONE
		#define D_MACOS
	#else
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
#else
	#error "unknown platform"
#endif

#endif

