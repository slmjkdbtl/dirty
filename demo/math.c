// wengwengweng

#define D_MATH_IMPL
#include <d_math.h>

int main() {
	d_rng rng = d_make_rng(1234);
	for (int i = 0; i < 10; i++) {
		printf("%f\n", d_rng_gen(&rng));
	}
}

