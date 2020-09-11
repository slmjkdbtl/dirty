// wengwengweng

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

int strcnt(const char *str, const char *key) {

	int len = strlen(key);
	int i = 0;
	int cnt = 0;

	for (i = 0; str[i] != '\0'; i++) {
		if (strstr(&str[i], key) == &str[i]) {
			cnt++;
			i += len - 1;
		}
	}

	return i;

}

char* strsub(const char *str, const char *old, const char *new) {

	int old_len = strlen(old);
	int new_len = strlen(new);
	int cnt = strcnt(str, old);
	char *res = malloc(strlen(str) + cnt * (new_len - old_len) + 1);
	int i = 0;

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

unsigned long d_hash(const char *str) {

	unsigned int hash = 0;

	while (*str) {
		hash = (hash << 7) + (hash >> 25) + *str++;
	}

	return hash + (hash >> 16);

}

#define FMT_BUFSIZE 256

const char* d_fmt(const char *fmt, ...) {

	static char buf[FMT_BUFSIZE];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, FMT_BUFSIZE, fmt, args);
	va_end(args);

	return buf;

}

