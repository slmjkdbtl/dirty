// wengwengweng

#define D_IMPL
#define D_CPU
#include <d_plat.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

#define countof(x) (sizeof(x) / sizeof((x)[0]))

#define WIDTH 128
#define HEIGHT 128
#define SCALE 4

d_mesh cube;
vec3 rot;

void init() {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = colori(0, 0, 0, 255),
	});

	d_vertex verts[] = {
		[0] = (d_vertex) {
			.pos = vec3f(1, 1, 1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colori(255, 255, 255, 255),
		},
		[1] = (d_vertex) {
			.pos = vec3f(-1, 1, 1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colori(0, 255, 255, 255),
		},
		[2] = (d_vertex) {
			.pos = vec3f(-1, -1, 1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colori(0, 0, 255, 255),
		},
		[3] = (d_vertex) {
			.pos = vec3f(1, -1, 1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colori(255, 0, 255, 255),
		},
		[4] = (d_vertex) {
			.pos = vec3f(1, 1, -1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colori(255, 255, 0, 255),
		},
		[5] = (d_vertex) {
			.pos = vec3f(-1, 1, -1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colori(0, 255, 0, 255),
		},
		[6] = (d_vertex) {
			.pos = vec3f(-1, -1, -1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colori(0, 0, 0, 255),
		},
		[7] = (d_vertex) {
			.pos = vec3f(1, -1, -1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colori(255, 0, 0, 255),
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

void frame() {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	vec2 mdpos = d_gfx_mouse_dpos();

	if (d_app_mouse_down(D_MOUSE_LEFT)) {
		rot.x += mdpos.y / 100;
		rot.y += mdpos.x / 100;
	}

	int s = d_gfx_width() / 4;

	d_gfx_clear();
	d_gfx_t_push();
	d_gfx_t_move3(vec3f(d_gfx_width() / 2, d_gfx_height() / 2, 0));
// 	d_gfx_t_rot_x(d_app_time());
// 	d_gfx_t_rot_y(d_app_time());
// 	d_gfx_t_rot_z(d_app_time());
	d_gfx_t_rot_x(rot.x);
	d_gfx_t_rot_y(rot.y);
	d_gfx_t_scale3(vec3f(s, -s, s));
// 	d_gfx_t_rot_z(d_app_time());
	d_draw_mesh(&cube, NULL);
	d_gfx_t_pop();
	d_gfx_present();

}

int main() {
	d_app_run((d_app_desc) {
		.title = "cube",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}
