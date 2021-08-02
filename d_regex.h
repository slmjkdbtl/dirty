// wengwengweng

// url: <protocal>://<host>/<path>

#ifndef D_REGEX_H
#define D_REGEX_H

#include <stdbool.h>

typedef struct {
	char* name;
	int name_len;
	char* txt;
	int txt_len;
} dr_region;

typedef struct {
	bool success;
	dr_region* regions;
	int num_regions;
} dr_res;

dr_res dr_match(char* regex, char* txt);

#endif

#ifdef D_IMPL
#define D_REGEX_IMPL
#endif

#ifdef D_REGEX_IMPL
#ifndef D_REGEX_IMPL_ONCE
#define D_REGEX_IMPL_ONCE

#include <stdlib.h>
#include <string.h>

dr_res dr_match(char* regex, char* txt) {
	char* r = regex;
	char* t = txt;
	int num_regions = 0;
	dr_region* regions = malloc(0);
	for (;;) {
		if (*r == '\0') {
			break;
		}
// 		printf("r: %s\n", r);
// 		printf("t: %s\n", t);
		if (*r == '<') {
			char* name_start = r + 1;
			while (*r != '>') r++;
			int name_len = r - name_start;
			char* txt_start = t;
			char stop = *++r;
			while (*t != stop) t++;
			int txt_len = t - txt_start;
			regions = realloc(regions, (num_regions + 1) * sizeof(dr_region));
			dr_region r = (dr_region) {
				.name = name_start,
				.name_len = name_len,
				.txt = txt_start,
				.txt_len = txt_len,
			};
			regions[num_regions++] = r;
		} else {
			if (*r != *t) {
				return (dr_res) {
					.success = false,
					.regions = regions,
					.num_regions = num_regions,
				};
			}
			r++;
			t++;
		}
	}
	return (dr_res) {
		.success = true,
		.regions = regions,
		.num_regions = num_regions,
	};
}

#endif
#endif
