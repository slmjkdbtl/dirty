// wengwengweng

#define D_IMPL
#define D_CPU
#include <d_plat.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

#define WIDTH 480
#define HEIGHT 480

d_mesh cube = (d_mesh) {
	.verts = {
		&(d_vertex) {
			.pos = { 1, 1, 1 },
		},
		&(d_vertex) {
			.pos = { -1, 1, 1 },
		},
	},
	.num_verts = 2,
};

// const VERTICES: [Vertex; 8] = [
// 	Vertex {
// 		pos: vec3!(1, 1, 1),
// 		normal: vec3!(0, 0, 0),
// 		uv: vec2!(0, 0),
// 		color: rgba!(1, 1, 1, 1),
// 	},
// 	Vertex {
// 		pos: vec3!(-1, 1, 1),
// 		normal: vec3!(0, 0, 0),
// 		uv: vec2!(0, 0),
// 		color: rgba!(0, 1, 1, 1),
// 	},
// 	Vertex {
// 		pos: vec3!(-1, -1, 1),
// 		normal: vec3!(0, 0, 0),
// 		uv: vec2!(0, 0),
// 		color: rgba!(0, 0, 1, 1),
// 	},
// 	Vertex {
// 		pos: vec3!(1, -1, 1),
// 		normal: vec3!(0, 0, 0),
// 		uv: vec2!(0, 0),
// 		color: rgba!(1, 0, 1, 1),
// 	},
// 	Vertex {
// 		pos: vec3!(1, 1, -1),
// 		normal: vec3!(0, 0, 0),
// 		uv: vec2!(0, 0),
// 		color: rgba!(1, 1, 0, 1),
// 	},
// 	Vertex {
// 		pos: vec3!(-1, 1, -1),
// 		normal: vec3!(0, 0, 0),
// 		uv: vec2!(0, 0),
// 		color: rgba!(0, 1, 0, 1),
// 	},
// 	Vertex {
// 		pos: vec3!(-1, -1, -1),
// 		normal: vec3!(0, 0, 0),
// 		uv: vec2!(0, 0),
// 		color: rgba!(0, 0, 0, 1),
// 	},
// 	Vertex {
// 		pos: vec3!(1, -1, -1),
// 		normal: vec3!(0, 0, 0),
// 		uv: vec2!(0, 0),
// 		color: rgba!(1, 0, 0, 1),
// 	},
// ];

// const INDICES: [u32; 36] = [
// 	0, 1, 2,
// 	0, 2, 3,
// 	4, 0, 3,
// 	4, 3, 7,
// 	4, 5, 1,
// 	4, 1, 0,
// 	5, 4, 7,
// 	5, 7, 6,
// 	1, 5, 6,
// 	1, 6, 2,
// 	3, 2, 6,
// 	3, 6, 7,
// ];

void init() {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = colori(0, 0, 0, 255),
	});

	printf("%f\n", cube.verts[1].pos.x);

}

void frame() {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	d_gfx_clear();
	d_draw_mesh(&cube, NULL);
	d_gfx_present();

}

int main() {
	d_app_run((d_app_desc) {
		.title = "cube",
		.init = init,
		.frame = frame,
		.width = WIDTH,
		.height = HEIGHT,
	});
}
