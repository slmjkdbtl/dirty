// wengwengweng

#ifndef D_UTILS_H
#define D_UTILS_H

#include "math.h"

unsigned long d_hash_str(const char *str);
// format text (one-off)
const char* d_fmt(const char *fmt, ...);
// format text (alloc)
char* d_fmta(const char *fmt, ...);

#endif

