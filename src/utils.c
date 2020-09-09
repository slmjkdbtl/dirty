// wengwengweng

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define HASH_INIT 2048
#define HASH_LIMIT 65536
#define FMT_BUFSIZE 256

char* strsub(const char* str, const char* old, const char* new) {

	int old_len = strlen(old);
	int new_len = strlen(new);
	int i = 0;
	int cnt = 0;

	for (i = 0; str[i] != '\0'; i++) {
		if (strstr(&str[i], old) == &str[i]) {
			cnt++;
			i += old_len - 1;
		}
	}

	char* res = malloc(i + cnt * (new_len - old_len) + 1);
	i = 0;

	while (*str) {
		if (strstr(str, old) == str) {
			strcpy(&res[i], new);
			i += new_len;
			str += old_len;
		} else {
			res[i++] = *str++;
		}
	}

	res[i] = '\0';

	return res;

}

unsigned long d_hash(const char* str) {

	unsigned long  hash = 0;
	int i = strlen(str);

	while (i--) {
		hash += (int)str[i] * HASH_INIT * (i + 1);
		hash %= HASH_LIMIT;
	}

	return hash;

}

const char* d_fmt(const char* fmt, ...) {

	static char buf[FMT_BUFSIZE];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, FMT_BUFSIZE, fmt, args);
	va_end(args);

	return buf;

}

