// wengwengweng

#ifndef D_FS_H
#define D_FS_H

#include <stdbool.h>
#include <stdlib.h>

// read resource file content (alloc)
char *d_read_text(const char *path);
unsigned char *d_read_bytes(const char *path, size_t *size);
char **d_read_dir(const char *path);
void d_free_dir(char **list);
bool d_is_file(const char *path);
bool d_is_dir(const char *path);

// read / write data file content (alloc)
char *d_data_read_text(const char *path);
unsigned char *d_data_read_bytes(const char *path, size_t *size);
void d_data_write_text(const char *path, const char *content);
void d_data_write_bytes(const char *path, const unsigned char *content, size_t size);
bool d_data_is_file(const char *path);
bool d_data_is_dir(const char *path);

char *d_extname(const char *path);
char *d_basename(const char *path);

#endif

