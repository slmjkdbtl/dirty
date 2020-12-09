// wengwengweng

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

#include <dirty/dirty.h>

#define PATH_MAX 256

#if defined(D_MACOS) || defined(D_IOS)
#import <Foundation/Foundation.h>
#endif

typedef struct {
	char res_path[PATH_MAX];
	char data_path[PATH_MAX];
} d_fs_ctx;

static d_fs_ctx d_fs;

void d_fs_init(const d_desc *desc) {

	if (desc->path) {
		sprintf(d_fs.res_path, "%s/", desc->path);
	} else {
#if defined(D_MACOS) || defined(D_IOS)
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		const char *res_path = [[[NSBundle mainBundle] resourcePath] UTF8String];
		sprintf(d_fs.res_path, "%s/", res_path);
		[pool drain];
#elif defined(D_WINDOWS)
		// TODO
#elif defined(D_LINUX)
		// TODO
#endif
	}

	if (desc->org && desc->name) {
#if defined(D_MACOS) || defined(D_IOS)
		const char *home = getenv("HOME");
		if (home) {
			sprintf(d_fs.data_path, "%s/Library/Application Support", home);
		}
#elif defined(D_WINDOWS)
		SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, d_fs.data_path);
		// TODO
#elif defined(D_LINUX)
		const char *home = getenv("HOME");
		const char *xdg_data = getenv("XDG_DATA_HOME");
		if (xdg_data) {
			sprintf(d_fs.data_path, "%s", xdg_data);
		} else if (home) {
			sprintf(d_fs.data_path, "%s/.local/share", home);
		}
#endif
		sprintf(d_fs.data_path, "%s/%s", d_fs.data_path, desc->org);
		mkdir(d_fs.data_path, 0700);
		sprintf(d_fs.data_path, "%s/%s/", d_fs.data_path, desc->name);
		mkdir(d_fs.data_path, 0700);
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
		d_fail("failed to read: '%s'\n", path);
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
	return read_text(d_fmt("%s%s", d_fs.res_path, path));
}

unsigned char *d_read_bytes(const char *path, size_t *size) {
	return read_bytes(d_fmt("%s%s", d_fs.res_path, path), size);
}

char **d_glob(const char *path, const char *ext) {
	return glob(d_fmt("%s%s", d_fs.res_path, path), ext);
}

bool d_is_file(const char *path) {
	return is_file(d_fmt("%s%s", d_fs.res_path, path));
}

bool d_is_dir(const char *path) {
	return is_dir(d_fmt("%s%s", d_fs.res_path, path));
}

char *d_data_read_text(const char *path) {
	return read_text(d_fmt("%s%s", d_fs.data_path, path));
}

unsigned char *d_data_read_bytes(const char *path, size_t *size) {
	return read_bytes(d_fmt("%s%s", d_fs.data_path, path), size);
}

bool d_data_is_file(const char *path) {
	return is_file(d_fmt("%s%s", d_fs.data_path, path));
}

bool d_data_is_dir(const char *path) {
	return is_dir(d_fmt("%s%s", d_fs.data_path, path));
}

// TODO: recursive mkdir?
void d_data_write_text(const char *path, const char *content) {
	write_text(d_fmt("%s%s", d_fs.data_path, path), content);
}

void d_data_write_bytes(const char *path, const unsigned char *content, size_t size) {
	write_bytes(d_fmt("%s%s", d_fs.data_path, path), content, size);
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

