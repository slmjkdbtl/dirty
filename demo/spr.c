// wengwengweng

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define D_IMPL
#include <d_spr.h>
#include <d_fs.h>

int main() {
	size_t size;
	size_t png_size;
	uint8_t* png_bytes = d_read_bytes("proggy_7x13.png", &png_size);
	uint8_t* bytes = d_spr_encode_from_png(
		png_bytes,
		png_size,
		7,
		13,
		" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~",
		0,
		NULL,
		&size
	);
	d_write_bytes("proggy.ds", bytes, size);
}
