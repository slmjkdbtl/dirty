#define D_IMPL
#include <d_lang.h>

int main(int argc, char** argv) {
	dt_vm vm = dt_vm_new();
	dt_load_libs(&vm);
	dt_eval(&vm, "@ (i \\ [3, 6, 2, 8]) print(i)");
	dt_vm_free(&vm);
	return 0;
}
