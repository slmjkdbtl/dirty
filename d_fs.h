// wengwengweng

// TODO: async

#ifndef D_FS_H
#define D_FS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

uint8_t*    d_read(char* path, size_t* osize);
char**      d_read_dir(char* path, char* ext);
void        d_free_dir(char** list);
bool        d_is_file(char* path);
bool        d_is_dir(char* path);
char*       d_extname(char* path);
char*       d_basename(char* path);
const char* d_res_dir();
char*       d_res_path(char* path);
char*       d_data_dir();

#endif

#ifdef D_IMPL
#define D_FS_IMPL
#endif

#ifdef D_FS_IMPL
#ifndef D_FS_IMPL_ONCE
#define D_FS_IMPL_ONCE

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#if defined(__APPLE__)
#import <Foundation/Foundation.h>
#endif

char* d_read_text(char* path, size_t* osize) {
	FILE* file = fopen(path, "r");
	if (!file) return NULL;
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* buffer = malloc(size + 1);
	fread(buffer, 1, size, file);
	buffer[size] = '\0';
	fclose(file);
	if (osize) *osize = size;
	return buffer;
}

uint8_t* d_read_bytes(char* path, size_t* osize) {
	FILE* file = fopen(path, "rb");
	if (!file) return NULL;
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	uint8_t* buffer = malloc(size);
	fread(buffer, 1, size, file);
	fclose(file);
	if (osize) *osize = size;
	return buffer;
}

bool d_write_text(char* path, char* text, size_t size) {
	FILE* file = fopen(path, "w");
	if (!file) return false;
	fwrite(text, 1, size ? size : strlen(text), file);
	fclose(file);
	return true;
}

bool write_bytes(char* path, uint8_t* data, size_t size) {
	FILE* file = fopen(path, "wb");
	if (!file) return false;
	fwrite(data, 1, size, file);
	fclose(file);
	return true;
}

bool d_is_file(char* path) {
	struct stat sb;
	return stat(path, &sb) == 0 && S_ISREG(sb.st_mode);
}

bool d_is_dir(char* path) {
	struct stat sb;
	return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
}

char** d_glob(char* path, char* ext) {

	DIR* dir = opendir(path);

	if (!dir) {
		return NULL;
	}

	struct dirent* entry;
	int num = 0;

	while ((entry = readdir(dir))) {
		char* aext = d_extname(entry->d_name);
		if (strcmp(ext, aext) == 0) {
			num++;
		}
		free(aext);
	}

	rewinddir(dir);
	char** list = malloc(sizeof(char*) * (num + 1));
	num = 0;

	while ((entry = readdir(dir))) {
		char* aext = d_extname(entry->d_name);
		if (strcmp(ext, aext) == 0) {
			list[num] = malloc(strlen(entry->d_name) + 1);
			strcpy(list[num], entry->d_name);
			num++;
		}
		free(aext);
	}

	list[num] = NULL;
	closedir(dir);

	return list;

}

void d_free_dir(char** list) {
	for (int i = 0; list[i] != NULL; i++) {
		free(list[i]);
	}
	free(list);
}

char* d_extname(char* path) {
	char* dot = strrchr(path, '.');
	if (!dot) {
		return NULL;
	}
	char* buf = malloc(strlen(dot + 1) + 1);
	strcpy(buf, dot + 1);
	return buf;
}

char* d_basename(char* path) {
	char* dot = strrchr(path, '.');
	char* slash = strrchr(path, '/');
	dot = dot ? dot : path + strlen(path);
	slash = slash ? slash + 1 : path;
	int len = dot - slash;
	char* buf = malloc(len + 1);
	strncpy(buf, slash, len);
	buf[len] = '\0';
	return buf;
}

const char* d_res_dir() {
#if defined(__APPLE__)
	@autoreleasepool {
		return [[[NSBundle mainBundle] resourcePath] UTF8String];
	}
#endif
	return "";
}

char* d_fmt(char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	size_t size = vsnprintf(NULL, 0, fmt, args);
	va_end(args);
	char* buf = malloc(size + 1);
	va_start(args, fmt);
	vsnprintf(buf, size + 1, fmt, args);
	va_end(args);
	return buf;
}

char* d_res_path(char* path) {
	const char* res_dir = d_res_dir();
	if (res_dir[0] == '\0') {
		return d_fmt("%s", path);
	} else {
		return d_fmt("%s/%s", res_dir, path);
	}
}

char* d_data_dir() {
#if defined(__APPLE__)
	const char* home = getenv("HOME");
	if (home) {
		return d_fmt("%s/Library/Application Support", home);
	} else {
		return d_fmt("~/Library/Application Support");
	}
#elif defined(__linux__) || defined(__unix__)
	char* home = getenv("HOME");
	char* xdg_home = getenv("XDG_DATA_HOME");
	if (xdg_home) {
		return d_fmt("%s", xdg_home);
	} else if (home) {
		return d_fmt("%s/.local/share", home);
	} else {
		return d_fmt("~/.local/share");
	}
#elif defined(_WIN32) || defined(_WIN64)
	// TODO
	return d_fmt("");
#endif
	return d_fmt("");
}

#endif
#endif
