#define D_IMPL
#include <d_math.h>
#include <d_fs.h>
#include <d_app.h>
#include <d_gfx.h>

#define WIDTH 120
#define HEIGHT 120
#define SCALE 4

void init(void) {
	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = d_colori(0, 0, 0, 255),
	});
}

void frame(void) {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	d_gfx_clear();
	d_blit_bg();

	d_line2 mouse_line = { d_vec2f(110, 10), d_gfx_mouse_pos() };

	d_line2 line = { d_vec2f(10, 10), d_vec2f(20, 90) };
	d_rect rect = { d_vec2f(40, 80), d_vec2f(80, 100) };
	d_circle circle = { d_vec2f(40, 30), 12 };
	d_poly poly = {
		.num_verts = 5,
		.verts = {
			d_vec2f(80, 40),
			d_vec2f(100, 60),
			d_vec2f(90, 75),
			d_vec2f(70, 70),
			d_vec2f(60, 50),
		},
	};

	bool col_line = d_col_line_line(mouse_line, line);
	bool col_rect = d_col_line_rect(mouse_line, rect);
	bool col_circle = d_col_line_circle(mouse_line, circle);
	bool col_poly = d_col_line_poly(mouse_line, poly);

	d_blit_line(line.p1, line.p2, d_colori(col_line ? 0 : 255, 255, 0, 255));
	d_blit_rect(rect.p1, rect.p2, d_colori(col_rect ? 0 : 255, 255, 0, 255));
	d_blit_circle(circle.center, circle.radius, d_colori(col_circle ? 0 : 255, 255, 0, 255));
	d_blit_poly(poly, d_colori(col_poly ? 0 : 255, 255, 0, 255));

	d_blit_line(mouse_line.p1, mouse_line.p2, d_colori(0, 0, 255, 255));

	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "col",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}
