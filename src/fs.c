// wengwengweng

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirty/dirty.h>

#include <SDL2/SDL.h>

#ifdef __APPLE__
#import <Foundation/Foundation.h>
#endif

char *d_fread(const char *path, int *o_size) {

	const char *rpath = d_rpath(path);

	if (!rpath) {
		return NULL;
	}

	FILE *file = fopen(rpath, "r");

	if (!file) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buffer = malloc(size + 1);
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

	return buffer;

}

unsigned char *d_fread_b(const char *path, int *o_size) {

	const char *rpath = d_rpath(path);

	if (!rpath) {
		return NULL;
	}

	FILE *file = fopen(rpath, "rb");

	if (!file) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char *buffer = malloc(size);
	size_t r_size = fread(buffer, 1, size, file);

	if (r_size != size) {
		free(buffer);
		return NULL;
	}

	if (o_size) {
		*o_size = size;
	}

	fclose(file);

	return buffer;

}

bool d_fexists(const char *path) {

	const char *rpath = d_rpath(path);
	bool exists = rpath != NULL;

	return exists;

}

const char *d_rpath(const char *path) {

	if (!path) {
		return NULL;
	}

	if (access(path, F_OK) != -1) {
		return path;
	}

#ifdef __APPLE__
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	const char *res_path = [[[NSBundle mainBundle] resourcePath] UTF8String];
	const char *cpath = d_fmt("%s/%s", res_path, path);
	[pool drain];

	if (access(cpath, F_OK) != -1) {
		return cpath;
	} else {
		return NULL;
	}
#endif

	return NULL;

}

const char *d_dpath(const char *org, const char *app, const char *path) {
	char *spath = SDL_GetPrefPath(org, app);
	const char *dpath = d_fmt("%s%s", spath, path);
	free(spath);
	return dpath;
}

