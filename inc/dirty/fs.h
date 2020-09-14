// wengwengweng

#ifndef D_FS_H
#define D_FS_H

#include <stdbool.h>

// read resource file content (alloc)
char *d_fread(const char *path, int *size);
unsigned char *d_fread_b(const char *path, int *size);
bool d_fexists(const char *path);

// read data file content (alloc)
char *d_data_read(const char *path, int *size);
unsigned char *d_data_read_b(const char *path, int *size);
void d_data_write(const char *path);
void d_data_write_b(const char *path);
bool d_data_exists(const char *path);

#endif

