// wengwengweng

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

#include <SDL2/SDL.h>

#include <dirty/dirty.h>

#define MAX_PATH_LEN 64

// TODO: namings

char *d_res_path(const char *path) {
	return d_fmta("%s%s", SDL_GetBasePath(), path);
}

char *d_data_path(const char *path) {
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

static char *read_text(const char *path) {

	FILE *file = fopen(path, "r");

	if (!file) {
		d_fail("failed to read: '%s'\n", path);
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
		d_fail("failed to read: '%s'\n", path);
		return NULL;
	}

	fclose(file);

	return buffer;

}

static unsigned char *read_bytes(const char *path, size_t *osize) {

	FILE *file = fopen(path, "rb");

	if (!file) {
		d_fail("failed to read: '%s'\n", path);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char *buffer = malloc(size);
	size_t r_size = fread(buffer, 1, size, file);

	if (r_size != size) {
		free(buffer);
		d_fail("failed to read: '%s'\n", path);
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
		d_fail("failed to write: '%s'\n", path);
		return false;
	}

	size_t size = strlen(text);
	size_t r_size = fwrite(text, 1, size, file);

	if (r_size != size) {
		d_fail("failed to write: '%s'\n", path);
		return false;
	}

	fclose(file);

	return true;

}

static bool write_bytes(const char *path, const unsigned char *data, size_t size) {

	FILE *file = fopen(path, "wb");

	if (!file) {
		d_fail("failed to write: '%s'\n", path);
		return false;
	}

	size_t r_size = fwrite(data, 1, size, file);

	if (r_size != size) {
		d_fail("failed to write: '%s'\n", path);
		return false;
	}

	fclose(file);

	return true;

}

typedef struct {
	char *(entries[MAX_PATH_LEN]);
	int num_entries;
} d_dir;

static d_dir *read_dir(const char *path) {

	DIR *dir = opendir(path);

	if (!dir) {
		d_fail("failed to read: '%s'\n", path);
		return NULL;
	}

	struct dirent *entry = readdir(dir);
	int num = 0;

	while (entry) {
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
			num++;
			printf("%s\n", entry->d_name);
		}
		entry = readdir(dir);
	}

	d_dir *ddir = malloc(sizeof(d_dir));
// 	ddir->entries = malloc(sizeof(char*) * num);

	closedir(dir);

	return ddir;

}

void d_free_dir(d_dir *dir) {
	free(dir->entries);
	free(dir);
}

char *d_read_text(const char *path) {
	char *rpath = d_res_path(path);
	char *content = read_text(rpath);
	free(rpath);
	return content;
}

unsigned char *d_read_bytes(const char *path, size_t *size) {
	char *rpath = d_res_path(path);
	unsigned char *data = read_bytes(rpath, size);
	free(rpath);
	return data;
}

char **d_read_dir(const char *path) {
	char *rpath = d_res_path(path);
	char **list = read_dir(rpath);
	free(rpath);
	return list;
}

bool d_is_file(const char *path) {
	struct stat sb;
	char *rpath = d_res_path(path);
	bool is = stat(rpath, &sb) == 0 && S_ISREG(sb.st_mode);
	free(rpath);
	return is;
}

bool d_is_dir(const char *path) {
	struct stat sb;
	char *rpath = d_res_path(path);
	bool is = stat(rpath, &sb) == 0 && S_ISDIR(sb.st_mode);
	free(rpath);
	return is;
}

char *d_data_read_text(const char *path) {
	char *dpath = d_data_path(path);
	char *data = read_text(dpath);
	free(dpath);
	return data;
}

unsigned char *d_data_read_bytes(const char *path, size_t *size) {
	char *dpath = d_data_path(path);
	unsigned char *data = read_bytes(dpath, size);
	free(dpath);
	return data;
}

bool d_data_is_file(const char *path) {
	struct stat sb;
	char *rpath = d_data_path(path);
	bool is = stat(rpath, &sb) == 0 && S_ISREG(sb.st_mode);
	free(rpath);
	return is;
}

bool d_data_is_dir(const char *path) {
	struct stat sb;
	char *rpath = d_data_path(path);
	bool is = stat(rpath, &sb) == 0 && S_ISDIR(sb.st_mode);
	free(rpath);
	return is;
}

void d_data_write_text(const char *path, const char *content) {
	char *dpath = d_data_path(path);
	write_text(dpath, content);
	free(dpath);
}

void d_data_write_bytes(const char *path, const unsigned char *content, size_t size) {
	char *dpath = d_data_path(path);
	write_bytes(dpath, content, size);
	free(dpath);
}

