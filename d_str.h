#ifndef D_STR_H
#define D_STR_H

#include <stddef.h>

int d_str_len(const char *c);
const char* d_str_char_next(const char *s);
const char* d_str_char_prev(const char *start, const char *s);
char* d_str_concat(const char *s1, const char *s2);
const char* d_str_char_at(const char *s, int i);

#endif

#ifdef D_IMPL
#define D_STR_IMPL
#endif

#ifdef D_STR_IMPL
#ifndef D_STR_IMPL_ONCE
#define D_STR_IMPL_ONCE

#include <string.h>
#include <stdlib.h>

static int d_str_char_size(const char* c) {
	uint8_t byte = (uint8_t) *c;
	if (byte < 0x80) return 1;
	else if ((byte >> 5) == 0x6) return 2;
	else if ((byte >> 4) == 0xE) return 3;
	else if ((byte >> 3) == 0x1E) return 4;
	return 1;
}

int d_str_len(const char* s) {
	size_t len = 0;
	while (*s) {
		s += d_str_char_size(s);
		len++;
	}
	return len;
}

const char* d_str_char_next(const char* s) {
	return s + d_str_char_size(s);
}

const char* d_str_char_prev(const char* start, const char *s) {
	while (s > start) {
		s--;
		if ((*s & 0xC0) != 0x80) {
			return s;
		}
	}
	return start;
}

char* d_str_concat(const char* s1, const char* s2) {
	size_t len1 = strlen(s1);
	size_t len2 = strlen(s2);
	char* result = malloc(len1 + len2 + 1);
	if (!result) return NULL;
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

const char* d_str_char_at(const char* s, int idx) {
	int len = d_str_len(s);
	if (idx >= len) return NULL;
	for (int i = 0; i < idx; i++) {
		s = d_str_char_next(s);
	}
	return s;
}

#endif
#endif
