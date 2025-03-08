#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define D_IMPL
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>
#include <d_physics.h>

#define WIDTH 240
#define HEIGHT 240
#define SCALE 2
#define SPEED 100.0f
#define ROT_SPEED 2.0f

d_t2 player_t = (d_t2) {
	.pos = (d_vec2) { 30, 30 },
	.scale = (d_vec2) { 1, 1 },
	.origin = (d_vec2) { 0, 0 },
	.rot = 0,
};

d_img img;

d_world world;
d_rect player_rect;
d_body* player_body;
d_body* rock1_body;
d_body* rock2_body;

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = d_colori(0, 0, 0, 255),
	});

	img = d_img_load(d_res_path("res/wizard.png"));
	world = d_world_new();

	player_t.origin = d_vec2_scale(d_vec2f(img.width, img.height), 0.5);

	player_rect = (d_rect) { d_vec2f(0, 0), d_vec2f(img.width, img.height) };
	player_body = d_world_add(&world, d_rect_to_poly(player_rect), NULL);
	rock1_body = d_world_add(&world, d_rect_to_poly((d_rect) { d_vec2f(100, 100), d_vec2f(150, 150) }), NULL);
	rock2_body = d_world_add(&world, d_rect_to_poly((d_rect) { d_vec2f(40, 180), d_vec2f(60, 200) }), NULL);

}

void frame(void) {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_F)) {
		d_app_set_fullscreen(!d_app_is_fullscreen());
	}

	float dt = d_app_dt();

	if (d_app_key_down(D_KEY_W)) {
		player_t.pos.y -= SPEED * dt;
	}

	if (d_app_key_down(D_KEY_S)) {
		player_t.pos.y += SPEED * dt;
	}

	if (d_app_key_down(D_KEY_A)) {
		player_t.pos.x -= SPEED * dt;
	}

	if (d_app_key_down(D_KEY_D)) {
		player_t.pos.x += SPEED * dt;
	}

	if (d_app_key_down(D_KEY_Q)) {
		player_t.rot -= ROT_SPEED * dt;
	}

	if (d_app_key_down(D_KEY_E)) {
		player_t.rot += ROT_SPEED * dt;
	}

	d_vec2 mpos = d_gfx_mouse_pos();

	player_body->shape = d_rect_transform(player_rect, d_t2_get_mat4(player_t));

	d_gfx_clear();
	d_blit_bg();

	d_draw_poly_outline(rock1_body->shape, d_colorx(0xffffffff));
	d_draw_poly_outline(rock2_body->shape, d_colorx(0xffffffff));

	d_world_check_reset(&world);

	while (true) {
		d_world_res res = d_world_check_next(&world);
		if (!res.collided) break;
		if (res.body_a == player_body) {
			player_t.pos = d_vec2_add(player_t.pos, res.dis);
		}
	}

	d_transform_push();
	d_t2_apply(player_t);
	d_draw_img(&img, D_WHITE);
	d_draw_poly_outline(d_rect_to_poly(player_rect), d_colorx(0x0000ffff));
	d_transform_pop();

	d_blit_circle(player_t.pos, 2, d_colorx(0xffff00ff));

	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "move",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}
