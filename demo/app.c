// wengwengweng

#define D_IMPL
#define D_CPU
#include <d_plat.h>
#include <d_math.h>
#include <d_gfx.h>
#include <d_app.h>

void frame() {
}

void init() {
	printf("123\n");
}

int main() {
	d_run((d_desc) {
		.title = "app",
		.init = init,
		.frame = frame,
		.width = 240,
		.height = 240,
	});
}

