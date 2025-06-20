#define D_IMPL
#include <d_lang.h>

int main(int argc, char** argv) {

	char* target = NULL;
	bool eval = false;

	for (int i = 1 ; i < argc; i++) {
		char* arg = argv[i];
		int len = strlen(arg);
		if (len >= 2 && arg[0] == '-' && arg[1] != '-') {
			switch (arg[1]) {
				case 'c':
					eval = true;
					break;
			}
		} else if (arg[0] != '-') {
			target = arg;
			break;
		}
	}

	dt_vm vm = dt_vm_new();
	dt_load_libs(&vm);

	if (target) {
		if (eval) {
			dt_run(&vm, target, NULL);
		} else {
			dt_runfile(&vm, target, NULL);
		}
	}

	// TODO: good place?
	if (vm.throwing) {
		dt_def_catch(&vm.err);
	}

	dt_vm_free(&vm);

	return 0;

}
