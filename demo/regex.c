#include <stdio.h>

#define D_IMPL
#include <d_regex.h>

int main() {
	printf("%d\n", dr_match("<protocol>://<host>/<path>", "http://space55.xyz/1").success);
}
