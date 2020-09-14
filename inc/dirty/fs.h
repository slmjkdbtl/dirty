// wengwengweng

#ifndef D_FS_H
#define D_FS_H

#include <stdbool.h>

// read resource file content (alloc)
char *d_fread(const char *path, int *size);
unsigned char *d_fread_b(const char *path, int *size);
bool d_fexists(const char *path);

// read / write data file content (alloc)
char *d_data_fread(const char *path, int *size);
unsigned char *d_data_fread_b(const char *path, int *size);
void d_data_fwrite(const char *path, const char *content);
void d_data_fwrite_b(const char *path, const unsigned char *content);
bool d_data_fexists(const char *path);

#endif

