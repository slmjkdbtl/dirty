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

static char *read_file(const char *path, int *osize) {

	FILE *file = fopen(path, "r");

	if (!file) {
		d_fail("failed to read %s\n", path);
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

	if (osize) {
		*osize = size;
	}

	fclose(file);

	return buffer;

}

static unsigned char *read_file_b(const char *path, int *osize) {

	FILE *file = fopen(path, "rb");

	if (!file) {
		d_fail("failed to read %s\n", path);
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

	if (osize) {
		*osize = size;
	}

	fclose(file);

	return buffer;

}

// TODO
static void write_file(const char *path, const char *content) {
	// ...
}

// TODO
static void write_file_b(const char *path, const unsigned char *content) {
	// ...
}

char *d_fread(const char *path, int *size) {
	char *rpath = d_rpath(path);
	char *data = read_file(rpath, size);
	free(rpath);
	return data;
}

unsigned char *d_fread_b(const char *path, int *size) {
	char *rpath = d_rpath(path);
	unsigned char *data = read_file_b(rpath, size);
	free(rpath);
	return data;
}

bool d_fexists(const char *path) {

	char *rpath = d_rpath(path);
	bool exists = file_exists(rpath);
	free(rpath);

	return exists;

}

char *d_data_fread(const char *path, int *size) {
	char *dpath = d_dpath(path);
	char *data = read_file(dpath, size);
	free(dpath);
	return data;
}

unsigned char *d_data_fread_b(const char *path, int *size) {
	char *dpath = d_dpath(path);
	unsigned char *data = read_file_b(dpath, size);
	free(dpath);
	return data;
}

bool d_data_fexists(const char *path) {

	char *dpath = d_dpath(path);
	bool exists = file_exists(dpath);
	free(dpath);

	return exists;

}

void d_data_fwrite(const char *path, const char *content) {
	char *dpath = d_dpath(path);
	write_file(dpath, content);
	free(dpath);
}

void d_data_fwrite_b(const char *path, const unsigned char *content) {
	char *dpath = d_dpath(path);
	write_file_b(dpath, content);
	free(dpath);
}

