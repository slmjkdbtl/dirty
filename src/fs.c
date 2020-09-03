// wengwengweng

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <dirty/fs.h>

#ifdef __APPLE__
#import <Foundation/Foundation.h>
#endif

char* d_fread(const char* path, int* o_size) {

	char* rpath = d_validate_path(path);

	if (!rpath) {
		return NULL;
	}

	FILE* file = fopen(rpath, "r");

	if (!file) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = malloc(size + 1);
	size_t r_size = fread(buffer, 1, size, file);

	buffer[size] = '\0';

	if (r_size != size) {
		free(buffer);
		return NULL;
	}

	if (o_size) {
		*o_size = size;
	}

	fclose(file);
	free(rpath);

	return buffer;

}

unsigned char* d_fread_b(const char* path, int* o_size) {

	char* rpath = d_validate_path(path);

	if (!rpath) {
		return NULL;
	}

	FILE* file = fopen(rpath, "rb");

	if (!file) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char* buffer = malloc(size);
	size_t r_size = fread(buffer, 1, size, file);

	if (r_size != size) {
		free(buffer);
		return NULL;
	}

	if (o_size) {
		*o_size = size;
	}

	fclose(file);
	free(rpath);

	return buffer;

}

bool d_fexists(const char* path) {

	char* rpath = d_validate_path(path);
	bool exists = rpath != NULL;
	free(rpath);

	return exists;

}

char* d_validate_path(const char* path) {

	if (!path) {
		return NULL;
	}

	if (access(path, F_OK) != -1) {
		char* cpath = malloc(sizeof(char) * strlen(path));
		strcpy(cpath, path);
		return cpath;
	}

#ifdef __APPLE__
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	const char* res_path = [[[NSBundle mainBundle] resourcePath] UTF8String];
	char* cpath = malloc(sizeof(char) * (strlen(res_path) + 1 + strlen(path)));
	sprintf(cpath, "%s/%s", res_path, path);
	[pool drain];

	if (access(cpath, F_OK) != -1) {
		return cpath;
	} else {
		free(cpath);
		return NULL;
	}
#endif

	return NULL;

}

