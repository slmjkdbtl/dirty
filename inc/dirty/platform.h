// wengwengweng

#ifndef D_PLATFORM_H
#define D_PLATFORM_H

#if defined(__APPLE__)
	#if TARGET_OS_IPHONE
		#define D_IOS
	#else
		#define D_MACOS
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

