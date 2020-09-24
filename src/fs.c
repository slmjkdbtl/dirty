// wengwengweng

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include <dirty/dirty.h>

// TODO: namings

static char *d_rpath(const char *path) {
	return d_fmta("%s%s", SDL_GetBasePath(), path);
}

static char *d_dpath(const char *path) {
#if defined(D_ORG_NAME) || defined(D_APP_NAME)
	char *spath = SDL_GetPrefPath(D_ORG_NAME, D_APP_NAME);
	const char *dpath = d_fmta("%s%s", spath, path);
	free(spath);
	return dpath;
#else
	d_fail("must define D_ORG_NAME and D_APP_NAME for data read / write");
	return NULL;
#endif
}

static bool file_exists(const char *path) {
	FILE *file = fopen(path, "r");
	bool exists = false;
	if (file) {
		exists = true;
		fclose(file);
	}
	return exists;
}

static void *read_file(const char *path, size_t *osize) {

	FILE *file = fopen(path, "rb");

	if (!file) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buffer = malloc(size + 1);
	size_t r_size = fread(buffer, 1, size, file);

	buffer[size] = 0;

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

// TODO
static void write_file(const char *path, const void *data, size_t size) {
	// ...
}

char *d_read_text(const char *path) {
	char *rpath = d_rpath(path);
	char *content = (char*)read_file(rpath, NULL);
	d_assert(content, "failed to read: '%s'\n", path);
	free(rpath);
	return content;
}

unsigned char *d_read_bytes(const char *path, size_t *size) {
	char *rpath = d_rpath(path);
	unsigned char *content = (unsigned char*)read_file(rpath, size);
	d_assert(content, "failed to read: '%s'\n", path);
	free(rpath);
	return content;
}

bool d_exists(const char *path) {
	char *rpath = d_rpath(path);
	bool exists = file_exists(rpath);
	free(rpath);
	return exists;
}

char *d_data_read_text(const char *path) {
	char *dpath = d_dpath(path);
	char *content = (char*)read_file(dpath, NULL);
	d_assert(content, "failed to read: '%s'\n", path);
	free(dpath);
	return content;
}

unsigned char *d_data_read_bytes(const char *path, size_t *size) {
	char *dpath = d_dpath(path);
	unsigned char *content = (unsigned char*)read_file(dpath, size);
	d_assert(content, "failed to read: '%s'\n", path);
	free(dpath);
	return content;
}

bool d_data_exists(const char *path) {
	char *dpath = d_dpath(path);
	bool exists = file_exists(dpath);
	free(dpath);
	return exists;
}

void d_data_write_text(const char *path, const char *data) {
	char *dpath = d_dpath(path);
	write_file(dpath, data, strlen(data));
	free(dpath);
}

void d_data_write_bytes(const char *path, const unsigned char *data, size_t size) {
	char *dpath = d_dpath(path);
	write_file(dpath, data, size);
	free(dpath);
}

