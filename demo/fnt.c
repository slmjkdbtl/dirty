// wengwengweng

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define D_IMPL
#include <d_fnt.h>

int main() {
	size_t size;
	uint8_t* bytes = d_fnt_encode_from_png("04b03_6x8.png", 6, 8, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", &size);
	FILE* file = fopen("04b03.df", "wb");
	fwrite(bytes, 1, size, file);
	fclose(file);
	d_fnt_decode_to_png(bytes, "04b03.png");
}
