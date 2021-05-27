// wengwengweng

// TODO: async

#ifndef D_FS_H
#define D_FS_H

#ifndef D_PLAT_H
#error 'd_fs.h' requires 'd_plat.h'
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
	const char *path;
	const char *data_org;
	const char *data_name;
} d_fs_desc;

typedef uint64_t d_fs_task;

void d_fs_init(d_fs_desc);
char *d_read_text(const char *path);
uint8_t *d_read_bytes(const char *path, size_t *size);
#ifdef D_FS_ASYNC
d_fs_task d_read_text_async(const char *path);
d_fs_task d_read_bytes_async(const char *path, size_t *size);
#endif
char **d_glob(const char *path, const char *ext);
void d_free_dir(char **list);
bool d_is_file(const char *path);
bool d_is_dir(const char *path);

char *d_data_read_text(const char *path);
uint8_t *d_data_read_bytes(const char *path, size_t *size);
void d_data_write_text(const char *path, const char *content);
void d_data_write_bytes(const char *path, const uint8_t *content, size_t size);
bool d_data_is_file(const char *path);
bool d_data_is_dir(const char *path);

char *d_extname(const char *path);
char *d_basename(const char *path);

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

#ifdef D_FS_ASYNC
#include <pthread.h>
static void *d_fs_thread(void *args);
#endif

#if defined(D_MACOS) || defined(D_IOS)
	#import <Foundation/Foundation.h>
#endif

#define D_PATH_MAX 256

typedef struct {
	char res_path[D_PATH_MAX];
	char data_path[D_PATH_MAX];
	d_fs_task last_task_id;
} d_fs_ctx;

static d_fs_ctx d_fs;

void d_fs_init(d_fs_desc desc) {

	if (desc.path) {
		sprintf(d_fs.res_path, "%s/", desc.path);
	} else {
#if defined(D_MACOS) || defined(D_IOS)
		@autoreleasepool {
			const char *res_path = [[[NSBundle mainBundle] resourcePath] UTF8String];
			sprintf(d_fs.res_path, "%s/", res_path);
		}
#elif defined(D_WINDOWS)
#elif defined(D_LINUX)
#endif
	}

	if (desc.data_org && desc.data_name) {
#if defined(D_MACOS) || defined(D_IOS)
		const char *home = getenv("HOME");
		if (home) {
			sprintf(d_fs.data_path, "%s/Library/Application Support", home);
		}
#elif defined(D_WINDOWS)
#elif defined(D_LINUX)
		const char *home = getenv("HOME");
		const char *xdg_data = getenv("XDG_DATA_HOME");
		if (xdg_data) {
			sprintf(d_fs.data_path, "%s", xdg_data);
		} else if (home) {
			sprintf(d_fs.data_path, "%s/.local/share", home);
		}
#endif
// 		sprintf(d_fs.data_path, "%s/%s", d_fs.data_path, desc.data_org);
// 		mkdir(d_fs.data_path, 0700);
// 		sprintf(d_fs.data_path, "%s/%s/", d_fs.data_path, desc.data_name);
// 		mkdir(d_fs.data_path, 0700);
	}

#ifdef D_FS_ASYNC

	pthread_t id;

	if (pthread_create(&id, NULL, d_fs_thread, NULL) < 0) {
		fprintf(stderr, "failed to start fs thread\n");
		return;
	}

#endif

}

static const char *fmt(const char *fmt, ...) {

	static char buf[D_PATH_MAX];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, D_PATH_MAX, fmt, args);
	va_end(args);

	return buf;

}

static char *read_text(const char *path) {

	FILE *file = fopen(path, "r");

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

	fclose(file);

	return buffer;

}

static uint8_t *read_bytes(const char *path, size_t *osize) {

	FILE *file = fopen(path, "rb");

	if (!file) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	uint8_t *buffer = malloc(size);
	size_t r_size = fread(buffer, 1, size, file);

	if (r_size != size) {
		free(buffer);
		return NULL;
	}

	if (osize) {
		*osize = size;
	}

	fclose(file);

	return buffer;

}

static bool write_text(const char *path, const char *text) {

	FILE *file = fopen(path, "w");

	if (!file) {
		fprintf(stderr, "failed to write: '%s'\n", path);
		return false;
	}

	size_t size = strlen(text);
	size_t r_size = fwrite(text, 1, size, file);

	if (r_size != size) {
		fprintf(stderr, "failed to write: '%s'\n", path);
		return false;
	}

	fclose(file);

	return true;

}

static bool write_bytes(const char *path, const uint8_t *data, size_t size) {

	FILE *file = fopen(path, "wb");

	if (!file) {
		fprintf(stderr, "failed to write: '%s'\n", path);
		return false;
	}

	size_t r_size = fwrite(data, 1, size, file);

	if (r_size != size) {
		fprintf(stderr, "failed to write: '%s'\n", path);
		return false;
	}

	fclose(file);

	return true;

}

static bool is_file(const char *path) {
	struct stat sb;
	return stat(path, &sb) == 0 && S_ISREG(sb.st_mode);
}

static bool is_dir(const char *path) {
	struct stat sb;
	return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
}

static char **glob(const char *path, const char *ext) {

	DIR *dir = opendir(path);

	if (!dir) {
		return NULL;
	}

	struct dirent *entry;
	int num = 0;

	while ((entry = readdir(dir))) {
		char *aext = d_extname(entry->d_name);
		if (strcmp(ext, aext) == 0) {
			num++;
		}
		free(aext);
	}

	rewinddir(dir);
	char **list = malloc(sizeof(char*) * (num + 1));
	num = 0;

	while ((entry = readdir(dir))) {
		char *aext = d_extname(entry->d_name);
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

void d_free_dir(char **list) {
	for (int i = 0; list[i] != NULL; i++) {
		free(list[i]);
	}
	free(list);
}

char *d_read_text(const char *path) {
	return read_text(fmt("%s%s", d_fs.res_path, path));
}

uint8_t *d_read_bytes(const char *path, size_t *size) {
	return read_bytes(fmt("%s%s", d_fs.res_path, path), size);
}

char **d_glob(const char *path, const char *ext) {
	return glob(fmt("%s%s", d_fs.res_path, path), ext);
}

bool d_is_file(const char *path) {
	return is_file(fmt("%s%s", d_fs.res_path, path));
}

bool d_is_dir(const char *path) {
	return is_dir(fmt("%s%s", d_fs.res_path, path));
}

char *d_data_read_text(const char *path) {
	return read_text(fmt("%s%s", d_fs.data_path, path));
}

uint8_t *d_data_read_bytes(const char *path, size_t *size) {
	return read_bytes(fmt("%s%s", d_fs.data_path, path), size);
}

bool d_data_is_file(const char *path) {
	return is_file(fmt("%s%s", d_fs.data_path, path));
}

bool d_data_is_dir(const char *path) {
	return is_dir(fmt("%s%s", d_fs.data_path, path));
}

// TODO: recursive mkdir?
void d_data_write_text(const char *path, const char *content) {
	write_text(fmt("%s%s", d_fs.data_path, path), content);
}

void d_data_write_bytes(const char *path, const uint8_t *content, size_t size) {
	write_bytes(fmt("%s%s", d_fs.data_path, path), content, size);
}

char *d_extname(const char *path) {
	const char *dot = strrchr(path, '.');
	if (!dot) {
		return NULL;
	}
	char *buf = malloc(strlen(dot + 1) + 1);
	strcpy(buf, dot + 1);
	return buf;
}

char *d_basename(const char *path) {
	const char *dot = strrchr(path, '.');
	const char *slash = strrchr(path, '/');
	dot = dot ? dot : path + strlen(path);
	slash = slash ? slash + 1 : path;
	int len = dot - slash;
	char *buf = malloc(len + 1);
	strncpy(buf, slash, len);
	buf[len] = '\0';
	return buf;
}

#ifdef D_FS_ASYNC

d_fs_task d_read_text_async(const char *path) {
	d_fs_task id = d_fs.last_task_id++;
	// TODO
	return id;
}

void *d_fs_aynsc_get(d_fs_task id) {
	// TODO
	return NULL;
}

static void *d_fs_thread(void *args) {
	// TODO
	return NULL;
}

#endif

#endif
#endif

