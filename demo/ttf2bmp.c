#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define FONT_SIZE 16
#define CHAR_START 32
#define CHAR_END 126
#define CHARS_PER_ROW 19

#define FMT_MAX 256

static char* fmt(char* fmt, ...) {

	static char buf[FMT_MAX];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, FMT_MAX, fmt, args);
	va_end(args);

	return buf;

}

bool streq(const char* s1, const char* s2) {
	return strcmp(s1, s2) == 0;
}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		printf("Usage: %s <font.ttf>\n", argv[0]);
		return 1;
	}

	int font_size = FONT_SIZE;
	int chars_per_row = CHARS_PER_ROW;
	bool embed = false;
	char embed_char = '*';
	bool hex = false;
	char* name = "font";
	char* ttf_file = NULL;

	for (int i = 1; i < argc; i++) {
		char* arg = argv[i];
		int len = strlen(arg);
		if (streq(arg, "-s") || streq(arg, "--size")) {
			if (i + 1 >= argc) {
				fprintf(stderr, "size not defined");
				return EXIT_FAILURE;
			}
			font_size = atoi(argv[i + 1]);
		} else if (streq(arg, "-r") || streq(arg, "--row")) {
			if (i + 1 >= argc) {
				fprintf(stderr, "row not defined");
				return EXIT_FAILURE;
			}
			chars_per_row = atoi(argv[i + 1]);
		} else if (streq(arg, "-c") || streq(arg, "--char")) {
			if (i + 1 >= argc) {
				fprintf(stderr, "char not defined");
				return EXIT_FAILURE;
			}
			embed_char = argv[i + 1][0];
		} else if (streq(arg, "-n") || streq(arg, "--name")) {
			if (i + 1 >= argc) {
				fprintf(stderr, "name not defined");
				return EXIT_FAILURE;
			}
			name = argv[i + 1];
		} else if (streq(arg, "-e") || streq(arg, "--embed")) {
			embed = true;
		} else if (streq(arg, "-h") || streq(arg, "--hex")) {
			hex = true;
		} else if (streq(arg, "-o") || streq(arg, "--output")) {
			// TODO
		} else if (!ttf_file) {
			ttf_file = arg;
		}
	}


	// load TTF file into memory
	FILE* font_file = fopen(ttf_file, "rb");
	if (!font_file) {
		perror("Failed to open font file");
		return 1;
	}

	fseek(font_file, 0, SEEK_END);
	size_t font_file_size = ftell(font_file);
	rewind(font_file);
	unsigned char* font_file_buf = (unsigned char*)malloc(font_file_size);
	fread(font_file_buf, 1, font_file_size, font_file);
	fclose(font_file);

	// initialize stb_truetype
	stbtt_fontinfo font;
	if (!stbtt_InitFont(&font, font_file_buf, stbtt_GetFontOffsetForIndex(font_file_buf, 0))) {
		printf("Failed to initialize font.\n");
		free(font_file_buf);
		return 1;
	}

	float scale = stbtt_ScaleForPixelHeight(&font, font_size);

	// get font metrics
	int ascent, descent;
	stbtt_GetFontVMetrics(&font, &ascent, &descent, NULL);
	ascent = (int)(ascent * scale);
	descent = (int)(descent * scale);

	// compute character width
	int max_advance_width = 0;

	for (int i = CHAR_START; i <= CHAR_END; i++) {
		int advance_width;
		stbtt_GetCodepointHMetrics(&font, i, &advance_width, NULL);
		if (advance_width > max_advance_width) {
			max_advance_width = advance_width;
		}
	}

	int char_width = (int)(max_advance_width * scale);
	int char_height = ascent - descent;

	int total_chars = CHAR_END - CHAR_START + 1;
	int rows = (total_chars + chars_per_row - 1) / chars_per_row;
	int img_width = chars_per_row * char_width;
	int img_height = rows * char_height;

	// allocate bitmap
	unsigned char* bitmap = (unsigned char*)calloc(img_width * img_height, 1);

	int x_offset = 0;
	int y_offset = 0;

	for (int cp = CHAR_START; cp <= CHAR_END; cp++) {

		int x0, y0, x1, y1;
		stbtt_GetCodepointBitmapBox(&font, cp, scale, scale, &x0, &y0, &x1, &y1);

		int char_bitmap_width = x1 - x0;
		int char_bitmap_height = y1 - y0;
		int baseline_offset = ascent + y0;

		unsigned char* char_bitmap = stbtt_GetCodepointBitmap(
			&font,
			0,
			scale,
			cp,
			&char_bitmap_width, &char_bitmap_height,
			0, 0
		);

		// draw character onto the grid
		for (int cy = 0; cy < char_bitmap_height; cy++) {
			for (int cx = 0; cx < char_bitmap_width; cx++) {
				int px = x_offset + cx;
				int py = y_offset + baseline_offset + cy;
				if (px < img_width && py < img_height) {
					bitmap[py * img_width + px] = char_bitmap[cy * char_bitmap_width + cx];
				}
			}
		}

		stbtt_FreeBitmap(char_bitmap, NULL);
		x_offset += char_width;

		if (x_offset >= img_width) {
			x_offset = 0;
			y_offset += char_height;
		}

	}

	if (embed) {
		if (hex) {
			// TODO
		} else {
			printf("const char font[%d][%d] = {\n", chars_per_row * char_width, rows * char_height);
			for (int y = 0; y < img_height; y++) {
				printf("    \"");
				for (int x = 0; x < img_width; x++) {
					int i = y * img_width + x;
					printf("%c", bitmap[i] == 0 ? ' ' : embed_char);
				}
				printf("\",\n");
			}
			printf("};\n");
			printf("\n");
			printf("int font_width = %d;\n", char_width);
			printf("int font_height = %d;\n", char_height);
		}
	} else {
		// save image
		char* file_name = fmt("%s_%dx%d.png", name, char_width, char_height);
		stbi_write_png(file_name, img_width, img_height, 1, bitmap, img_width);
		printf("bitmap font saved as %s\n", file_name);
	}

	free(font_file_buf);
	free(bitmap);

	return 0;

}
