#define D_IMPL
#include <d_plat.h>
#include <d_fs.h>

int main(void) {
	printf("%s\n", d_read_text("Makefile", NULL));
}

