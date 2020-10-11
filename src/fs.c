// wengwengweng

#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

#include <SDL2/SDL.h>

#include <dirty/dirty.h>

typedef struct {
	char base_path[PATH_MAX];
	char data_path[PATH_MAX];
} d_fs_ctx;

static d_fs_ctx d_fs;

void d_fs_init(d_desc *desc) {
	if (desc->path) {
		strcpy(d_fs.base_path, desc->path);
	} else {
		char *rpath = SDL_GetBasePath();
		strcpy(d_fs.base_path, rpath);
		SDL_free(rpath);
	}
	if (desc->org && desc->name) {
		char *dpath = SDL_GetPrefPath(desc->org, desc->name);
		strcpy(d_fs.data_path, dpath);
		SDL_free(dpath);
	}
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
	char *(entries[PATH_MAX]);
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
	return read_text(d_fmt("%s%s", d_fs.base_path, path));
}

unsigned char *d_read_bytes(const char *path, size_t *size) {
	return read_bytes(d_fmt("%s%s", d_fs.base_path, path), size);
}

char **d_read_dir(const char *path) {
	char **list = read_dir(d_fmt("%s%s", d_fs.base_path, path));
	return list;
}

bool d_is_file(const char *path) {
	struct stat sb;
	return stat(d_fmt("%s%s", d_fs.base_path, path), &sb) == 0 && S_ISREG(sb.st_mode);
}

bool d_is_dir(const char *path) {
	struct stat sb;
	return stat(d_fmt("%s%s", d_fs.base_path, path), &sb) == 0 && S_ISDIR(sb.st_mode);
}

char *d_data_read_text(const char *path) {
	return read_text(d_fmt("%s%s", d_fs.data_path, path));
}

unsigned char *d_data_read_bytes(const char *path, size_t *size) {
	return read_bytes(d_fmt("%s%s", d_fs.data_path, path), size);
}

bool d_data_is_file(const char *path) {
	struct stat sb;
	bool is = stat(d_fmt("%s%s", d_fs.base_path, path), &sb) == 0 && S_ISREG(sb.st_mode);
	return is;
}

bool d_data_is_dir(const char *path) {
	struct stat sb;
	bool is = stat(d_fmt("%s%s", d_fs.base_path, path), &sb) == 0 && S_ISDIR(sb.st_mode);
	return is;
}

void d_data_write_text(const char *path, const char *content) {
	write_text(d_fmt("%s%s", d_fs.data_path, path), content);
}

void d_data_write_bytes(const char *path, const unsigned char *content, size_t size) {
	write_bytes(d_fmt("%s%s", d_fs.data_path, path), content, size);
}

