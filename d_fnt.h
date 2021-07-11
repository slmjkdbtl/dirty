// wengwengweng

// TODO: u16 #chars
// TODO: utf8

// dirty bitmap font format
//  -----------------------------------
// | magic      u8[9] (d1Rtyf0Nt)      |
//  -----------------------------------
// | w          u8                     |
//  -----------------------------------
// | h          u8                     |
//  -----------------------------------
// | #chars     u8                     |
//  -----------------------------------
// | chars      u8[#chars]             |
//  -----------------------------------
// | bitmap     u8[#chars * w * h / 8] |
//  -----------------------------------

#ifndef D_FNT_H
#define D_FNT_H

void d_fnt_decode(uint8_t* bytes);

uint8_t* d_fnt_encode_from_png(
	char* path,
	uint8_t gw,
	uint8_t gh,
	char* chars,
	size_t* osize
);

#endif

#ifdef D_IMPL
#define D_FNT_IMPL
#endif

#ifdef D_FNT_IMPL
#ifndef D_FNT_IMPL_ONCE
#define D_FNT_IMPL_ONCE

// impl

void d_fnt_decode_to_png(
	uint8_t* bytes,
	char* path
) {

	uint8_t gw = bytes[9];
	uint8_t gh = bytes[10];
	uint8_t chars_size = bytes[11];
	size_t bitmap_size = ceil(chars_size * gw * gh / 8.0f);
	uint8_t* bitmap_buf = bytes + 9 + 1 + 1 + 1 + chars_size;
	uint8_t* pixels = calloc(chars_size * gw * gh * 4, 1);

	// TODO: support multiple columns
	for (int i = 0; i < bitmap_size; i++) {
		for (int j = 0; j < 8; j++) {
			if (bitmap_buf[i] & (1 << (7 - j))) {
				pixels[(i * 8 + j) * 4 + 3] = 255;
			}
		}
	}

	stbi_write_png(
		path,
		gw,
		gh * chars_size,
		4,
		pixels,
		gw * 4
	);

}

uint8_t* d_fnt_encode_from_png(
	char* path,
	uint8_t gw,
	uint8_t gh,
	char* chars,
	size_t* osize
) {

	int w, h;
	uint8_t* pixels = stbi_load(path, &w, &h, NULL, 4);
	uint8_t num_cols = w / gw;
	uint8_t num_rows = h / gh;
	size_t chars_size = strlen(chars);
	size_t bitmap_size = ceil(chars_size * gw * gh / 8.0f);

	size_t total_size =
		9              // magic
		+ 1            // w
		+ 1            // h
		+ 1            // #chars
		+ chars_size   // chars
		+ bitmap_size; // bitmap

	if (osize) {
		*osize = total_size;
	}

	uint8_t* buf = malloc(total_size);
	uint8_t* chars_buf = buf + 9 + 1 + 1 + 1;
	uint8_t* bitmap_buf = chars_buf + chars_size;

	memcpy(buf, "d1Rtyf0Nt", 9);
	buf[9] = gw;
	buf[10] = gh;
	buf[11] = chars_size;
	memcpy(chars_buf, chars, chars_size);
	memset(bitmap_buf, 0, bitmap_size);

	int cursor = 0;

	for (int row = 0; row < num_rows; row++) {
		for (int col = 0; col < num_cols; col++) {
			for (int y = 0; y < gh; y++) {
				for (int x = 0; x < gw; x++) {
					int i = (row * gh + y) * w + col * gw + x;
					if (pixels[i * 4 + 3] > 0) {
						bitmap_buf[cursor / 8] |= (1 << (7 - cursor % 8));
					}
					cursor++;
				}
			}
		}
	}

	return buf;

}

#endif
#endif
