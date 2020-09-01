// wengwengweng

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#ifdef __APPLE__
#import <Foundation/Foundation.h>
#endif

char* d_fread(const char* path, size_t* o_size) {

	FILE* file = fopen(path, "r");

	if (!file) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = malloc(size);
	fread(buffer, 1, size, file);

	fclose(file);
	if (o_size) {
		*o_size = size;
	}

	return buffer;

}

unsigned char* d_fread_b(const char* path, size_t* o_size) {

	FILE* file = fopen(path, "rb");

	if (!file) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char* buffer = malloc(size);
	fread(buffer, 1, size, file);

	fclose(file);
	if (o_size) {
		*o_size = size;
	}

	return buffer;

}

bool d_fexists(const char* path) {
	return access(path, F_OK) != -1;
}

// TODO: validate_path
static const char* res_path() {
#ifdef __APPLE__
// 	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	NSBundle* bundle = [NSBundle mainBundle];
	NSString* path = [bundle resourcePath];
// 	[pool drain];
	return [ path UTF8String ];
#else
	return "";
#endif
}

