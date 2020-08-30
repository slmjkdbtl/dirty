// wengwengweng

#include <stdbool.h>
#include <SDL2/SDL.h>

const char* d_fread(const char* path) {

	SDL_RWops* rw = SDL_RWFromFile(path, "rb");

	if (rw == NULL) {
		return NULL;
	}

	int size = SDL_RWsize(rw);

	char* res = (char*)malloc(size + 1);
	Sint64 nb_read_total = 0, nb_read = 1;
	char* buf = res;

	while (nb_read_total < size && nb_read != 0) {

		nb_read = SDL_RWread(rw, buf, 1, (size - nb_read_total));
		nb_read_total += nb_read;
		buf += nb_read;

	}

	if (nb_read_total != size) {
		free(res);
		return NULL;
	}

	res[nb_read_total] = '\0';
	SDL_RWclose(rw);

	return (const char*)res;

}

bool d_fexists(const char* path) {
	return SDL_RWFromFile(path, "rb") != NULL;
}

