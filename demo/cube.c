#define D_IMPL
#include <d_plat.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

#define countof(x) (sizeof(x) / sizeof((x)[0]))

#define WIDTH 480
#define HEIGHT 480
#define SCALE 4

d_mesh cube;
d_vec3 rot = (d_vec3) { -0.4, 0.8, 0 };

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.scale = SCALE,
		.clear_color = d_colori(0, 0, 0, 255),
	});

	d_vertex verts[] = {
		[0] = (d_vertex) {
			.pos = d_vec3f(1, 1, 1),
			.normal = d_vec3f(0, 0, 0),
			.uv = d_vec2f(0, 0),
			.color = d_colori(255, 255, 255, 255),
		},
		[1] = (d_vertex) {
			.pos = d_vec3f(-1, 1, 1),
			.normal = d_vec3f(0, 0, 0),
			.uv = d_vec2f(0, 0),
			.color = d_colori(0, 255, 255, 255),
		},
		[2] = (d_vertex) {
			.pos = d_vec3f(-1, -1, 1),
			.normal = d_vec3f(0, 0, 0),
			.uv = d_vec2f(0, 0),
			.color = d_colori(0, 0, 255, 255),
		},
		[3] = (d_vertex) {
			.pos = d_vec3f(1, -1, 1),
			.normal = d_vec3f(0, 0, 0),
			.uv = d_vec2f(0, 0),
			.color = d_colori(255, 0, 255, 255),
		},
		[4] = (d_vertex) {
			.pos = d_vec3f(1, 1, -1),
			.normal = d_vec3f(0, 0, 0),
			.uv = d_vec2f(0, 0),
			.color = d_colori(255, 255, 0, 255),
		},
		[5] = (d_vertex) {
			.pos = d_vec3f(-1, 1, -1),
			.normal = d_vec3f(0, 0, 0),
			.uv = d_vec2f(0, 0),
			.color = d_colori(0, 255, 0, 255),
		},
		[6] = (d_vertex) {
			.pos = d_vec3f(-1, -1, -1),
			.normal = d_vec3f(0, 0, 0),
			.uv = d_vec2f(0, 0),
			.color = d_colori(0, 0, 0, 255),
		},
		[7] = (d_vertex) {
			.pos = d_vec3f(1, -1, -1),
			.normal = d_vec3f(0, 0, 0),
			.uv = d_vec2f(0, 0),
			.color = d_colori(255, 0, 0, 255),
		},
	};

	d_index indices[] = {
		0, 1, 2,
		0, 2, 3,
		4, 0, 3,
		4, 3, 7,
		4, 5, 1,
		4, 1, 0,
		5, 4, 7,
		5, 7, 6,
		1, 5, 6,
		1, 6, 2,
		3, 2, 6,
		3, 6, 7,
	};

	cube = d_mesh_new(verts, countof(verts), indices, countof(indices));
	d_mesh_gen_normals(&cube);

}

void frame(void) {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	d_vec2 mdpos = d_gfx_mouse_dpos();

	if (d_app_mouse_down(D_MOUSE_LEFT)) {
		rot.x -= mdpos.y / 100;
		rot.y -= mdpos.x / 100;
	}

	int s = d_gfx_width() / 4;
	float t = d_app_time();

	d_gfx_clear();
	d_transform_push();
	d_transform_pos3(d_vec3f(d_gfx_width() / 2.0, d_gfx_height() / 2.0, 0));
	d_transform_rot_x(rot.x);
	d_transform_rot_y(rot.y);
	d_transform_scale3(d_vec3f(s, -s, s));
	d_draw_mesh(&cube, NULL);
	d_transform_pop();
	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "cube",
		.init = init,
		.frame = frame,
		.width = WIDTH,
		.height = HEIGHT,
	});
}
