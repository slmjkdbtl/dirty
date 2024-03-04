#define D_IMPL
#include <d_lang.h>

int main(int argc, char** argv) {
	dt_vm vm = dt_vm_new();
	dt_load_libs(&vm);
	dt_run(&vm, "@ (i \\ [3, 6, 2, 8]) print(i)", NULL);
	dt_vm_free(&vm);
	return 0;
}
