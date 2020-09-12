// wengwengweng

#ifndef D_FS_H
#define D_FS_H

#include <stdbool.h>

// read a text file (alloc)
char *d_fread(const char *path, int *size);
// read a binary file (alloc)
unsigned char *d_fread_b(const char *path, int *size);
// check if a file exists
bool d_fexists(const char *path);
// get resource path (e.g. macOS bundle Contents/Resources/*)
const char *d_rpath(const char *path);
// get data path (for save files etc.)
const char *d_dpath(const char *org, const char *app, const char *path);

#endif

