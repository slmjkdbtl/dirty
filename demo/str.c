#define D_IMPL
#include <d_str.h>
#include <stdio.h>

int main(void) {
	char* s1 = "hello";
	char* s2 = "你好";
	char* s3 = "こんにちわ";
	printf("%s, %d\n", s1, d_str_len(s1));
	printf("%s, %d\n", s2, d_str_len(s2));
	printf("%s, %d\n", s3, d_str_len(s3));
	printf("%s\n", d_str_char_at(s2, 1));
}
