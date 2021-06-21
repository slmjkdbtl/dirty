#define D_IMPL
#include <d_lang.h>

int main(int argc, char **argv) {
	if (argc >= 2) {
		dt_dofile(argv[1]);
	}
	return 0;
}
