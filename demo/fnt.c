#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define D_IMPL
#include <d_fnt.h>
#include <d_fs.h>

int main() {
	size_t size;
	uint8_t* bytes = d_fnt_encode_from_png("proggy_7x13.png", 7, 13, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", &size);
	d_write_bytes("proggy.df", bytes, size);
	d_fnt_decode_to_png(bytes, "proggy.png");
}
