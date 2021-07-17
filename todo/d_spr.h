// wengwengweng

// dirty sprite format
//  -----------------------------------
// | magic      u8[11] (d1Rty$pR1te)   |
//  -----------------------------------
// | w          u8                     |
//  -----------------------------------
// | h          u8                     |
//  -----------------------------------
// | #col       u8                     |
//  -----------------------------------
// | #row       u8                     |
//  -----------------------------------
// | charmap    u8[#col * #row]        |
//  -----------------------------------
// | #anims     u8                     |
//  -----------------------------------
// | anims      anim[#anims]           |
//  -----------------------------------
// | png        u8[]                   |
//  -----------------------------------

// anim
//  -----------------------------------
// | name       char[16]               |
//  -----------------------------------
// | start      u8                     |
//  -----------------------------------
// | end        u8                     |
//  -----------------------------------
// | loop       u8                     |
//  -----------------------------------
// | pingpong   u8                     |
//  -----------------------------------

#ifndef D_SPR_H
#define D_SPR_H

#include <stdbool.h>

typedef struct {
	char name[16];
	bool loop;
	bool pingpong;
	int start;
	int end;
} d_spr_anim;

// typedef struct {
// 	int w;
// 	int h;
// 	color* p;
// } d_img;

// typedef struct {
// 	int* charmap;
// 	int num_frames;
// 	d_img* frames;
// 	int num_anims;
// 	d_spr_anim* anims;
// } d_spr;

#endif

#ifdef D_IMPL
#define D_SPR_IMPL
#endif

#ifdef D_SPR_IMPL
#ifndef D_SPR_IMPL_ONCE
#define D_SPR_IMPL_ONCE

uint8_t* d_spr_encode_from_png(
	uint8_t* png_bytes,
	size_t png_size,
	uint8_t gw,
	uint8_t gh,
	char* chars,
	int num_anims,
	d_spr_anim* anims,
	size_t* osize
) {

	int w;
	int h;
	uint8_t* pixels = stbi_load_from_memory(png_bytes, png_size, &w, &h, NULL, 4);
	uint8_t num_cols = w / gw;
	uint8_t num_rows = h / gh;
	int num_frames = num_cols * num_rows;
	int anims_size = num_anims * (16 + 1 + 1 + 1 + 1);

	size_t total_size =
		11             // magic
		+ 1            // w
		+ 1            // h
		+ 1            // #col
		+ 1            // #row
		+ num_frames   // charmap
		+ 1            // #anims TODO
		+ anims_size   // anims TODO
		+ png_size;    // png

	if (osize) {
		*osize = total_size;
	}

	uint8_t* buf = malloc(total_size);
	uint8_t* charmap = buf + 11 + 1 + 1 + 1 + 1;
	uint8_t* png_buf = charmap + 1 + anims_size;

	memcpy(buf, "d1Rty$pR1te", 11);
	buf[11] = gw;
	buf[12] = gh;
	buf[13] = num_cols;
	buf[14] = num_rows;
	memset(charmap, 0, num_frames);
	memcpy(charmap, chars, strlen(chars));
	memcpy(png_buf, png_bytes, png_size);

	return buf;

}

#endif
#endif
