#include <stdio.h>
#define D_MATH_IMPL
#include <d_math.h>

int main(void) {
	d_rng rng = d_rng_new(1234);
	for (int i = 0; i < 10; i++) {
		printf("%f\n", d_rng_gen(&rng));
	}
}
