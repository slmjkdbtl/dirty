// wengwengweng

#ifndef D_FS_H
#define D_FS_H

#include <stdbool.h>

// read resource file content (alloc)
char *d_read_text(const char *path);
unsigned char *d_read_bytes(const char *path, int *size);
bool d_exists(const char *path);

// read / write data file content (alloc)
char *d_data_read_text(const char *path);
unsigned char *d_data_read_bytes(const char *path, int *size);
void d_data_write_text(const char *path, const char *content);
void d_data_write_bytes(const char *path, const unsigned char *content);
bool d_data_exists(const char *path);

#endif

