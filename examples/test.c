// wengwengweng

#include <stdlib.h>
#include <dirty/dirty.h>

d_mesh tri_mesh;
d_tex tex;
d_canvas canvas;

static void frame() {

	if (d_key_pressed(D_KEY_F)) {
		d_set_fullscreen(!d_fullscreen());
	}

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit(EXIT_SUCCESS);
	}

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

	d_index indices[] = {
		0, 1, 2,
	};

	d_scale_xy(vec2f(2.0, 2.0));
// 	d_draw_raw(verts, 3, indices, 3);
	d_draw_tex(&tex, quadu());
// 	d_draw_text("hihi", 120.0);
// 	d_draw_text("1234567890123456789012345678901234567890123456789012345678901234567890123456790", 8.0);
// 	d_draw_canvas(&canvas);
// 	d_draw_mesh(&tri_mesh);

}

int main() {

	d_init("test", 640, 480);

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

	d_index indices[] = {
		0, 1, 2,
	};

	tri_mesh = d_make_mesh(verts, 3, indices, 3);
	int size;
	unsigned char* content = d_fread_b("res/acid2.png", &size);
	tex = d_parse_tex(content, size);
	canvas = d_make_canvas(d_width(), d_height());

	d_use_canvas(&canvas);
	d_draw_mesh(&tri_mesh);
	d_use_canvas(NULL);

	d_run(frame);

}

