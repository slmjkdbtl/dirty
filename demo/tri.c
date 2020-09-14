// wengwengweng

#include <stdlib.h>
#include <dirty/dirty.h>

int main() {

	d_init("tri", 640, 480);

	d_vertex verts[] = {
		{
			.pos = vec3f(0.0, 120.0, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(0.0, 0.0),
			.color = colorf(1.0, 0.0, 0.0, 1.0)
		},
		{
			.pos = vec3f(-160.0, -120.0, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(0.0, 0.0),
			.color = colorf(0.0, 1.0, 0.0, 1.0)
		},
		{
			.pos = vec3f(160.0, -120.0, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(0.0, 0.0),
			.color = colorf(0.0, 0.0, 1.0, 1.0)
		},
	};

	d_index indices[] = { 0, 1, 2, };

	while (d_running()) {

		d_clear();

		if (d_key_pressed(D_KEY_ESC)) {
			d_quit();
		}

		d_draw_raw(verts, 3, indices, 3, NULL);
		d_frame();

	}

}

