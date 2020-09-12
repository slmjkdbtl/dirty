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

unsigned long d_hash_str(const char *str) {

	unsigned int hash = 0;

	while (*str) {
		hash = (hash << 7) + (hash >> 25) + *str++;
	}

	return hash + (hash >> 16);

}

#define TEXT_BUF_SIZE 65536

const char* d_fmt(const char *fmt, ...) {

	static char buf[TEXT_BUF_SIZE];
	static int idx;

	va_list args;

	va_start(args, fmt);
	int size = vsnprintf(NULL, 0, fmt, args) + 1;
	va_end(args);

	if (idx + size >= TEXT_BUF_SIZE) {
		idx = 0;
		memset(buf, 0, TEXT_BUF_SIZE);
	}

	va_start(args, fmt);
	vsnprintf(&buf[idx], size, fmt, args);
	va_end(args);

	idx += size;

	return &buf[idx - size];

}

