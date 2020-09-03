// wengwengweng

#ifndef D_FS_H
#define D_FS_H

#include <stdbool.h>

char* d_fread(const char*, int*);
unsigned char* d_fread_b(const char*, int*);
bool d_fexists(const char*);
char* d_validate_path(const char* path);

#endif

