// wengwengweng

#ifndef D_UTILS_H
#define D_UTILS_H

unsigned long d_hash_str(const char *str);
// format text (for one-off situations)
const char* d_fmt(const char *fmt, ...);
// format text (allocates)
char* d_fmt_m(const char *fmt, ...);

#endif

