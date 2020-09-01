// wengwengweng

#ifndef D_FS_H
#define D_FS_H

#include <stdbool.h>

char* d_fread(const char*, size_t*);
unsigned char* d_fread_b(const char*, size_t*);
bool d_fexists(const char*);

#endif

