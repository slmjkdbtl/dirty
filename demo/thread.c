// wengwengweng

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#define D_IMPL
#include <d_thread.h>

bool loaded;

void *load(void *arg) {
	sleep(1);
	loaded = true;
}

int main() {
	d_texec(load, NULL);
	printf("start loading\n");
	while (!loaded) {
		printf("loading\n");
	}
	printf("loaded\n");
}
