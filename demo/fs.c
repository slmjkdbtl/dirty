#define D_IMPL
#include <d_fs.h>

int main(void) {
	printf("%s\n", d_read_text("res/fav_pokemon.txt", NULL));
}

